#define NOMINMAX

#include "GPUObjectSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Manager/ComponentManager.h>

//============================================================================
//	GPUObjectSystem classMethods
//============================================================================

void GPUObjectSystem::Init(ID3D12Device* device, Asset* asset) {

	meshRegistry_ = std::make_unique<MeshRegistry>();
	meshRegistry_->Init(device, asset);

	instancedMeshBuffer_ = std::make_unique<InstancedMeshBuffer>();
	instancedMeshBuffer_->SetDevice(device);
}

void GPUObjectSystem::CreateMesh(const std::string& modelName) {

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName);

	// 最大instance数
	const uint32_t kMaxInstanceNum = 4096;

	// instancingデータ作成
	instancedMeshBuffer_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

void GPUObjectSystem::CreateObject2D(uint32_t id, SpriteComponent* sprite, ID3D12Device* device) {

	size_t index = object2DBuffers_.size();

	object2DBufferToIndex_[id] = index;
	indexToObject2DBuffer_.emplace_back(id);

	object2DBuffers_.resize(std::max(static_cast<uint32_t>(object2DBuffers_.size()), id + 1));

	// buffer作成
	object2DBuffers_[id].matrix.CreateConstBuffer(device);
	object2DBuffers_[id].material.CreateConstBuffer(device);

	// spriteの情報を取得
	object2DBuffers_[id].sprite.sprite = sprite;
}

void GPUObjectSystem::RemoveObject2D(uint32_t id) {

	if (!Algorithm::Find(object2DBufferToIndex_, id)) {
		return;
	}

	size_t index = object2DBufferToIndex_.at(id);
	size_t lastIndex = object2DBuffers_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(object2DBuffers_[index], object2DBuffers_[lastIndex]);

		// 交換されたentityIdを更新
		uint32_t movedEntityId = indexToObject2DBuffer_[lastIndex];
		object2DBufferToIndex_[movedEntityId] = index;
		indexToObject2DBuffer_[index] = movedEntityId;
	}

	// 末尾を削除
	object2DBuffers_.pop_back();
	indexToObject2DBuffer_.pop_back();
	object2DBufferToIndex_.erase(id);
}

void GPUObjectSystem::Update() {

	// buffer更新
	// 3D
	UpdateObject3D();
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

		for (const auto* mat : materialPtrs) {

			materials.emplace_back(*mat);
		}

		const Transform3DComponent* transform = componentManager->GetComponent<Transform3DComponent>(id);
		TransformationMatrix matrix{};

		// bufferに送るデータをセット
		instancedMeshBuffer_->SetUploadData(transform->GetInstancingName(),
			transform->matrix, materials);
	}

	instancedMeshBuffer_->Update();
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