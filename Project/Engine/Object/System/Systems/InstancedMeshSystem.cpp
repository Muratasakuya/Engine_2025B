#include "InstancedMeshSystem.h"

//============================================================================
//	include
//============================================================================
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

		const std::string& instancingName = transform->GetInstancingName();
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