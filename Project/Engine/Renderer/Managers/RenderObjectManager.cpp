#include "RenderObjectManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	RenderObjectManager classMethods
//============================================================================

void RenderObjectManager::CreateObject3D(EntityID id, ModelComponent* model, ID3D12Device* device) {

	object3DBuffers_[id].matrix.CreateConstBuffer(device);

	if (model->isAnimation) {

		object3DBuffers_[id].materials.resize(model->animationModel->GetMeshNum());
	} else {

		object3DBuffers_[id].materials.resize(model->model->GetMeshNum());
	}
	for (auto& material : object3DBuffers_[id].materials) {

		material.CreateConstBuffer(device);
	}
	object3DBuffers_[id].model = model;
}

void RenderObjectManager::RemoveObject3D(EntityID id) {

	object3DBuffers_.erase(id);
}

void RenderObjectManager::Update() {

	auto componentManager = ComponentManager::GetInstance();

	const std::vector<EntityID> id = componentManager->GetEntityIDs();
	if (id.empty()) {
		return;
	}

	// entityごとのGPUデータ転送
	for (auto it = id.begin(); it < id.end(); ++it) {

		object3DBuffers_[*it].matrix.TransferData(componentManager->GetComponent<Transform3DComponent>(*it)->matrix);

		for (uint32_t index = 0; index < object3DBuffers_[*it].materials.size(); ++index) {

			object3DBuffers_[*it].materials[index].TransferData(*componentManager->GetComponent<Material>(*it));
		}
	}
}

const std::unordered_map<BlendMode, std::vector<const RenderObjectManager::Object3DForGPU*>>&
RenderObjectManager::GetSortedObject3Ds() const {

	if (needsSorting_) {
		RebuildBlendModeCache();
	}
	return sortedObject3Ds_;
}

void RenderObjectManager::RebuildBlendModeCache() const {

	sortedObject3Ds_.clear();

	for (auto& [id, obj] : object3DBuffers_) {

		const auto& renderingData = obj.model->renderingData;
		// 描画無効
		if (!renderingData.drawEnable) {
			continue;
		}
		sortedObject3Ds_[renderingData.blendMode].emplace_back(&obj);
	}
	needsSorting_ = false;
}