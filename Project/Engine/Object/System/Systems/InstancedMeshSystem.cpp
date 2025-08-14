#include "InstancedMeshSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Object/Core/ObjectPoolManager.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingScene.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Config.h>

//============================================================================
//	InstancedMeshSystem classMethods
//============================================================================

InstancedMeshSystem::InstancedMeshSystem(ID3D12Device* device, Asset* asset,
	DxCommand* dxCommand) : device_{ device }, asset_{ asset }, dxCommand_{ dxCommand } {

	meshRegistry_ = std::make_unique<MeshRegistry>();
	meshRegistry_->Init(device_, asset_);

	instancedBuffer_ = std::make_unique<InstancedMeshBuffer>();
	instancedBuffer_->Init(device_, asset_);
}

InstancedMeshSystem::~InstancedMeshSystem() {

	// 処理されている非同期処理をすべて停止させる
	StopBuildWorker();
}

void InstancedMeshSystem::StartBuildWorker() {

	buildWorker_.Start([this](MeshBuildJob&& job) {

		// ジョブ開始
		runningJobs_.fetch_add(1, std::memory_order_relaxed);
		// キュー分を減算
		pendingJobs_.fetch_sub(1, std::memory_order_relaxed);
		if (job.skinned) {

			// 骨ありメッシュ
			meshRegistry_->RegisterMesh(job.name, true, job.maxInstance);
			std::scoped_lock lock(instancedMutex_);
			instancedBuffer_->Create(meshRegistry_->GetMeshes().at(job.name).get(),
				job.name, job.maxInstance);
		} else {

			// 静的メッシュ
			meshRegistry_->RegisterMesh(job.name, false, job.maxInstance);
			std::scoped_lock lock(instancedMutex_);
			instancedBuffer_->Create(meshRegistry_->GetMeshes().at(job.name).get(),
				job.name, job.maxInstance);
		}
		// ジョブ終了
		runningJobs_.fetch_sub(1, std::memory_order_relaxed);
		});
}

void InstancedMeshSystem::StopBuildWorker() {

	buildWorker_.Stop();
}

void InstancedMeshSystem::RequestBuild(const std::string& modelName,
	uint32_t maxInstStatic, uint32_t maxInstSkinned) {

	// 作成済み、キューに設定済みなら処理しない
	if (IsReady(modelName) || requested_.count(modelName)) {
		return;
	}

	// 骨があるか判定
	const bool skinned = !asset_->GetModelData(modelName).skinClusterData.empty();
	const uint32_t maxInstatnce = skinned ? maxInstSkinned : maxInstStatic;

	auto& queue = buildWorker_.RefAsyncQueue();
	if (queue.IsClearCondition([&](const MeshBuildJob& j) { return j.name == modelName; })) {
		return;
	}
	// 処理キューの追加
	queue.AddQueue(MeshBuildJob{ modelName, skinned, maxInstatnce });
	requested_.insert(modelName);
	pendingJobs_.fetch_add(1, std::memory_order_relaxed);
}

void InstancedMeshSystem::RequestBuildForScene(Scene scene) {

	// シーンファイルから作成するメッシュのリストを取得する
	const auto& modelNames = asset_->GetPreloadModels(scene);
	for (auto& modelName : modelNames) {

		RequestBuild(modelName);
	}
}

bool InstancedMeshSystem::IsReady(const std::string& name) const {

	// メッシュ作成済みかどうか
	const auto& meshes = meshRegistry_->GetMeshes();
	if (!meshes.contains(name)) {
		return false;
	}
	// バッファ作成済みかどうか
	const auto& insttance = instancedBuffer_->GetInstancingData();
	return insttance.contains(name);
}

bool InstancedMeshSystem::IsBuilding() const {

	// どれかが1以上なら処理中
	return 0 < (pendingJobs_.load(std::memory_order_relaxed) +
		runningJobs_.load(std::memory_order_relaxed));
}

float InstancedMeshSystem::GetBuildProgressForScene(Scene scene) const {

	const auto& modelNames = asset_->GetPreloadModels(scene);
	if (modelNames.empty()) {
		return 1.0f;
	}

	uint32_t ready = 0;
	for (auto& model : modelNames) {
		if (IsReady(model)) {
			++ready;
		}
	}
	return std::clamp(ready / static_cast<float>(modelNames.size()), 0.0f, 1.0f);
}

void InstancedMeshSystem::CreateStaticMesh(const std::string& modelName) {

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName, false, 0);
	// instancingデータ作成
	instancedBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, Config::kMaxInstanceNum);
}

void InstancedMeshSystem::CreateSkinnedMesh(const std::string& modelName) {

	// 最大instance数
	const uint32_t kMaxInstanceNum = 16;

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName, true, kMaxInstanceNum);
	// instancingデータ作成
	instancedBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

Archetype InstancedMeshSystem::Signature() const {

	Archetype arch{};
	arch.set(ObjectPoolManager::GetTypeID<Transform3D>());
	arch.set(ObjectPoolManager::GetTypeID<Material>());
	return arch;
}

void InstancedMeshSystem::Update(ObjectPoolManager& ObjectPoolManager) {

	// bufferクリア
	instancedBuffer_->Reset();

	const auto& view = ObjectPoolManager.View(Signature());

	for (const auto& object : view) {

		auto* transform = ObjectPoolManager.GetData<Transform3D>(object);
		auto* materials = ObjectPoolManager.GetData<Material, true>(object);
		auto* animation = ObjectPoolManager.GetData<SkinnedAnimation>(object);

		// 未作成の場合スキップ
		const std::string& instancingName = transform->GetInstancingName();
		if (!IsReady(instancingName)) {
			continue;
		}
		instancedBuffer_->SetUploadData(
			instancingName, transform->matrix, *materials, *animation);
	}

	// buffer転送
	instancedBuffer_->Update(dxCommand_);
}

std::vector<RayTracingInstance> InstancedMeshSystem::CollectRTInstances(const RaytracingScene* scene) const {

	std::vector<RayTracingInstance> out;
	UINT instanceCounter = 0;

	const auto& meshes = meshRegistry_->GetMeshes();
	const auto& instanceMap = instancedBuffer_->GetInstancingData();

	for (const auto& [modelName, meshPtr] : meshes) {

		auto instIt = instanceMap.find(modelName);
		if (instIt == instanceMap.end()) {
			continue;
		}

		const MeshInstancingData& instData = instIt->second;
		const uint32_t subMeshCount = meshPtr->GetMeshCount();
		const size_t numInst = instData.matrixUploadData.size();
		const std::vector<std::vector<LightingForGPU>>& lightingData = instData.lightingUploadData;
		for (uint32_t j = 0; j < numInst; ++j) {

			const Matrix4x4& world = instData.matrixUploadData[j].world;
			for (uint32_t sub = 0; sub < subMeshCount; ++sub) {

				RayTracingInstance instance{};
				const LightingForGPU& lighting = lightingData[sub][j];
				instance.matrix = world;
				instance.instanceID = ++instanceCounter;
				instance.mask = lighting.castShadow ? 0xFF : 0x01;
				instance.hitGroupIdx = 0;
				instance.flags = 0;

				instance.blas = scene->GetBLASResource(meshPtr.get(), sub);
				out.emplace_back(instance);
			}
		}
	}

	return out;
}