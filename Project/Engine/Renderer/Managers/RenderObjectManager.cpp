#define NOMINMAX

#include "RenderObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	RenderObjectManager classMethods
//============================================================================

void RenderObjectManager::Init(ID3D12Device* device, SRVManager* srvManager) {

	instancedMesh_ = std::make_unique<InstancedMesh>();
	instancedMesh_->Init(device, srvManager);
}

void RenderObjectManager::CreateObject3D(EntityID id, const std::optional<std::string>& instancingName,
	ModelComponent* model, ID3D12Device* device) {

	size_t index = object3DBuffers_.size();

	object3DBufferToIndex_[id] = index;
	indexToObject3DBuffer_.emplace_back(id);

	object3DBuffers_.resize(std::max(static_cast<EntityID>(object3DBuffers_.size()), id + 1));

	// instancing処理
	if (instancingName.has_value()) {

		// 最大instance数
		const uint32_t kMaxInstanceNum = 4096;

		// instancingデータ作成
		instancedMesh_->Create(*model, *instancingName, kMaxInstanceNum);

		object3DBuffers_[id].model.renderingData.instancingEnable = true;
		object3DBuffers_[id].model.renderingData.instancingName = *instancingName;
	}
	// 通常描画処理
	else {

		// buffer作成
		object3DBuffers_[id].matrix.CreateConstBuffer(device);

		if (model->isAnimation) {

			object3DBuffers_[id].materials.resize(model->animationModel->GetMeshNum());
		} else {

			object3DBuffers_[id].materials.resize(model->model->GetMeshNum());
		}
		for (auto& material : object3DBuffers_[id].materials) {

			material.CreateConstBuffer(device);
		}

		// model情報の取得
		object3DBuffers_[id].model.model = model->model.get();
		object3DBuffers_[id].model.renderingData.instancingEnable = false;
		object3DBuffers_[id].model.animationModel = model->animationModel.get();
		object3DBuffers_[id].model.isAnimation = model->isAnimation;
		object3DBuffers_[id].model.renderingData = model->renderingData;
	}
}

void RenderObjectManager::RemoveObject3D(EntityID id) {

	if (!Algorithm::Find(object3DBufferToIndex_, id)) {
		return;
	}

	size_t index = object3DBufferToIndex_.at(id);
	size_t lastIndex = object3DBuffers_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(object3DBuffers_[index], object3DBuffers_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToObject3DBuffer_[lastIndex];
		object3DBufferToIndex_[movedEntityId] = index;
		indexToObject3DBuffer_[index] = movedEntityId;
	}

	// 末尾を削除
	object3DBuffers_.pop_back();
	indexToObject3DBuffer_.pop_back();
	object3DBufferToIndex_.erase(id);
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
	for (size_t index = 0; index < indexToObject3DBuffer_.size(); ++index) {
		EntityID id = indexToObject3DBuffer_[index];

		if (!object3DBuffers_[index].model.renderingData.instancingEnable) {

			object3DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform3DComponent>(id)->matrix);

			for (uint32_t mIndex = 0; mIndex < object3DBuffers_[index].materials.size(); ++mIndex) {

				object3DBuffers_[index].materials[mIndex].TransferData(*componentManager->GetComponent<Material>(id));
			}
		} else {

			std::vector<Material*> materialPtrs = componentManager->GetComponentList<Material>(id);
			std::vector<Material> materials;
			materials.reserve(materialPtrs.size());

			for (const auto* mat : materialPtrs) {

				materials.emplace_back(*mat);
			}

			const ModelComponent* model = componentManager->GetComponent<ModelComponent>(id);
			const Transform3DComponent* transform = componentManager->GetComponent<Transform3DComponent>(id);
			TransformationMatrix matrix{};

			matrix = transform->matrix;
			// 描画しないのであればmatrixを0.0fにする
			// modelを1個ごとに操作できないため
			if (!model->renderingData.drawEnable) {

				matrix.world = Matrix4x4::Zero();
				matrix.worldInverseTranspose = Matrix4x4::Zero();
			}

			// bufferに送るデータをセット
			instancedMesh_->SetComponent(object3DBuffers_[index].model.renderingData.instancingName,
				matrix, materials);
		}
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