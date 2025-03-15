#include "RenderObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	RenderObjectManager classMethods
//============================================================================

void RenderObjectManager::CreateObject3D(EntityID id, ModelComponent* model, ID3D12Device* device) {

	// buffer作成
	object3DBuffers_.emplace_back();

	object3DBuffers_[id].matrix.CreateConstBuffer(device, 0);

	if (model->isAnimation) {

		object3DBuffers_[id].materials.resize(model->animationModel->GetMeshNum());
	} else {

		object3DBuffers_[id].materials.resize(model->model->GetMeshNum());
	}
	for (auto& material : object3DBuffers_[id].materials) {

		material.CreateConstBuffer(device, 5);
	}

	// model情報の取得
	object3DBuffers_[id].model.model = model->model.get();
	object3DBuffers_[id].model.animationModel = model->animationModel.get();
	object3DBuffers_[id].model.isAnimation = model->isAnimation;
	object3DBuffers_[id].model.renderingData = model->renderingData;
}

void RenderObjectManager::RemoveObject3D(EntityID id) {

	// buffer削除
	object3DBuffers_.erase(object3DBuffers_.begin() + id);
}

void RenderObjectManager::Update() {

	auto componentManager = ComponentManager::GetInstance();

	// entityごとのGPUデータ転送
	for (uint32_t index = 0; index < componentManager->GetEntityCount(); ++index) {

		object3DBuffers_[index].matrix.TransferData(componentManager->GetComponent<Transform3DComponent>(index)->matrix);

		for (uint32_t mIndex = 0; mIndex < object3DBuffers_[index].materials.size(); ++mIndex) {

			object3DBuffers_[index].materials[mIndex].TransferData(*componentManager->GetComponent<Material>(index));
		}
	}
}