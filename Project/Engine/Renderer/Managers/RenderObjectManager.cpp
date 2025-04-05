#define NOMINMAX

#include "RenderObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	RenderObjectManager classMethods
//============================================================================

void RenderObjectManager::Init(ID3D12Device* device, SRVManager* srvManager, Asset* asset) {

	meshRegistry_ = std::make_unique<MeshRegistry>();
	meshRegistry_->Init(device, asset);

	instancedMesh_ = std::make_unique<InstancedMesh>();
	instancedMesh_->Init(device, srvManager);

}

void RenderObjectManager::CreateMesh(const std::string& modelName) {

	// meshの作成、登録
	meshRegistry_->RegisterMesh(modelName);

	// 最大instance数
	const uint32_t kMaxInstanceNum = 4096;

	// instancingデータ作成
	instancedMesh_->Create(meshRegistry_->GetMeshes().at(modelName).get(),
		modelName, kMaxInstanceNum);
}

void RenderObjectManager::CreateObject2D(EntityID id, SpriteComponent* sprite, ID3D12Device* device) {

	size_t index = object2DBuffers_.size();

	object2DBufferToIndex_[id] = index;
	indexToObject2DBuffer_.emplace_back(id);

	object2DBuffers_.resize(std::max(static_cast<EntityID>(object2DBuffers_.size()), id + 1));

	// buffer作成
	object2DBuffers_[id].matrix.CreateConstBuffer(device);
	object2DBuffers_[id].material.CreateConstBuffer(device);

	// spriteの情報を取得
	object2DBuffers_[id].sprite.sprite = sprite;
}

void RenderObjectManager::RemoveObject2D(EntityID id) {

	if (!Algorithm::Find(object2DBufferToIndex_, id)) {
		return;
	}

	size_t index = object2DBufferToIndex_.at(id);
	size_t lastIndex = object2DBuffers_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(object2DBuffers_[index], object2DBuffers_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToObject2DBuffer_[lastIndex];
		object2DBufferToIndex_[movedEntityId] = index;
		indexToObject2DBuffer_[index] = movedEntityId;
	}

	// 末尾を削除
	object2DBuffers_.pop_back();
	indexToObject2DBuffer_.pop_back();
	object2DBufferToIndex_.erase(id);
}

void RenderObjectManager::Update() {

	// buffer更新
	// 3D
	UpdateObject3D();
	// 2D
	UpdateObject2D();
}

void RenderObjectManager::UpdateObject3D() {

	auto componentManager = ComponentManager::GetInstance();

	instancedMesh_->Reset();

	// entityごとのGPUデータ転送
	for (EntityID index = 0; index < componentManager->GetEntityCount(ComponentType::Object3D); ++index) {

		EntityID id = componentManager->GetEntityIndex(ComponentType::Object3D, index);

		std::vector<Material*> materialPtrs = componentManager->GetComponentList<Material>(id);
		std::vector<Material> materials;
		materials.reserve(materialPtrs.size());

		for (const auto* mat : materialPtrs) {

			materials.emplace_back(*mat);
		}

		const Transform3DComponent* transform = componentManager->GetComponent<Transform3DComponent>(id);
		TransformationMatrix matrix{};

		// bufferに送るデータをセット
		instancedMesh_->SetComponent(transform->GetInstancingName(),
			transform->matrix, materials);
	}

	instancedMesh_->Update();
}

void RenderObjectManager::UpdateObject2D() {

	auto componentManager = ComponentManager::GetInstance();

	// entityごとのGPUデータ転送
	for (size_t index = 0; index < indexToObject2DBuffer_.size(); ++index) {
		EntityID id = indexToObject2DBuffer_[index];

		object2DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform2DComponent>(id)->matrix);
		object2DBuffers_[index].material.TransferData(*componentManager->GetComponent<SpriteMaterial>(id));
	}
}