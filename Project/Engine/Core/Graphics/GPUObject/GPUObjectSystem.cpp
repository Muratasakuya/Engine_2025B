#define NOMINMAX

#include "GPUObjectSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ECS/ComponentManager.h>

//============================================================================
//	GPUObjectSystem classMethods
//============================================================================

void GPUObjectSystem::Init(ID3D12Device* device, Asset* asset) {

	meshRegistry_ = std::make_unique<MeshRegistry>();
	meshRegistry_->Init(device, asset);

	instancedMeshBuffer_ = std::make_unique<InstancedMeshBuffer>();
	instancedMeshBuffer_->SetDevice(device);

	// sceneBuffer作成
	sceneConstBuffer_ = std::make_unique<SceneConstBuffer>();
	sceneConstBuffer_->Create(device);
}

void GPUObjectSystem::CreateMesh(const std::string& modelName) {

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName);

	// 最大instance数
	const uint32_t kMaxInstanceNum = 16192;

	// instancingデータ作成
	instancedMeshBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

void GPUObjectSystem::CreateObject2D(uint32_t entityId, SpriteComponent* sprite, ID3D12Device* device) {

	size_t index = object2DBuffers_.size();

	// buffer追加
	object2DBuffers_.emplace_back();
	// buffer作成
	object2DBuffers_.back().matrix.CreateConstBuffer(device);
	object2DBuffers_.back().material.CreateConstBuffer(device);
	// spriteの情報を取得
	object2DBuffers_.back().sprite = sprite;

	// index設定
	object2DBufferToIndex_[entityId] = index;
	indexToObject2DBuffer_.push_back(entityId);
}

void GPUObjectSystem::RemoveObject2D(uint32_t entityId) {

	// 見つかなければ削除しない
	if (!Algorithm::Find(object2DBufferToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = object2DBufferToIndex_.at(entityId);
	size_t lastIndex = object2DBuffers_.size() - 1;

	// bufferIndex削除
	SwapToPopbackObject2D(entityId);

	// buffer削除
	std::swap(object2DBuffers_[index], object2DBuffers_[lastIndex]);
	object2DBuffers_.pop_back();
}

void GPUObjectSystem::CreateEffect(uint32_t entityId,
	PrimitiveMeshComponent* primitiveMesh, ID3D12Device* device) {

	size_t index = effectBuffers_.size();

	// buffer追加
	effectBuffers_.emplace_back();
	// buffer作成
	effectBuffers_.back().matrix.CreateConstBuffer(device);
	effectBuffers_.back().material.CreateConstBuffer(device);
	// primitiveMeshの情報を取得
	effectBuffers_.back().primitiveMesh = primitiveMesh->GetPrimitiveMesh();

	// index設定
	effectBufferToIndex_[entityId] = index;
	indexToEffectBuffer_.push_back(entityId);
}

void GPUObjectSystem::RemoveEffect(uint32_t entityId) {

	// 見つかなければ削除しない
	if (!Algorithm::Find(effectBufferToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = effectBufferToIndex_.at(entityId);
	size_t lastIndex = effectBuffers_.size() - 1;

	// bufferIndex削除
	SwapToPopbackEffect(entityId);

	// buffer削除
	std::swap(effectBuffers_[index], effectBuffers_[lastIndex]);
	effectBuffers_.pop_back();
}

void GPUObjectSystem::SwapToPopbackObject2D(uint32_t entityId) {

	size_t index = object2DBufferToIndex_.at(entityId);
	size_t lastIndex = object2DBuffers_.size() - 1;

	if (index != lastIndex) {

		// 交換されたentityIdを更新
		uint32_t movedEntityId = indexToObject2DBuffer_[lastIndex];
		object2DBufferToIndex_[movedEntityId] = index;
		indexToObject2DBuffer_[index] = movedEntityId;
	}

	// 末尾を削除
	indexToObject2DBuffer_.pop_back();
	object2DBufferToIndex_.erase(entityId);
}

void GPUObjectSystem::SwapToPopbackEffect(uint32_t entityId) {

	size_t index = effectBufferToIndex_.at(entityId);
	size_t lastIndex = effectBuffers_.size() - 1;

	if (index != lastIndex) {

		// 交換されたentityIdを更新
		uint32_t movedEntityId = indexToEffectBuffer_[lastIndex];
		effectBufferToIndex_[movedEntityId] = index;
		indexToEffectBuffer_[index] = movedEntityId;
	}

	// 末尾を削除
	indexToEffectBuffer_.pop_back();
	effectBufferToIndex_.erase(entityId);
}

void GPUObjectSystem::Update(CameraManager* cameraManager,
	LightManager* lightManager) {

	// buffer更新
	// scene
	sceneConstBuffer_->Update(cameraManager, lightManager);
	// 3D
	UpdateObject3D();
	// effect
	UpdateEffect();
	// 2D
	UpdateObject2D();
}

void GPUObjectSystem::UpdateObject3D() {

	auto componentManager = ComponentManager::GetInstance();

	instancedMeshBuffer_->Reset();

	const auto& entityList = componentManager->GetEntityList(ComponentType::Object3D);

	// entityごとのGPUデータ転送
	for (uint32_t id : entityList) {

		std::vector<MaterialComponent*> materialPtrs = componentManager->GetComponentList<MaterialComponent>(id);

		std::vector<MaterialComponent> materials;
		materials.reserve(materialPtrs.size());

		for (const auto* material : materialPtrs) {

			materials.emplace_back(*material);
		}

		const Transform3DComponent* transform = componentManager->GetComponent<Transform3DComponent>(id);

		// bufferに送るデータをセット
		instancedMeshBuffer_->SetUploadData(transform->GetInstancingName(),
			transform->matrix, materials);
	}

	instancedMeshBuffer_->Update();
}

void GPUObjectSystem::UpdateEffect() {

	auto componentManager = ComponentManager::GetInstance();

	// entityごとのGPUデータ転送
	for (size_t index = 0; index < indexToEffectBuffer_.size(); ++index) {
		uint32_t id = indexToEffectBuffer_[index];

		effectBuffers_[index].matrix.TransferData(componentManager->GetComponent<EffectTransformComponent>(id)->matrix);
		effectBuffers_[index].material.TransferData(componentManager->GetComponent<EffectMaterialComponent>(id)->material);
	}
}

void GPUObjectSystem::UpdateObject2D() {

	auto componentManager = ComponentManager::GetInstance();

	// entityごとのGPUデータ転送
	for (size_t index = 0; index < indexToObject2DBuffer_.size(); ++index) {
		uint32_t id = indexToObject2DBuffer_[index];

		object2DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform2DComponent>(id)->matrix);
		object2DBuffers_[index].material.TransferData(*componentManager->GetComponent<SpriteMaterial>(id));
	}
}