#include "Transform3DManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	Transform3DManager classMethods
//============================================================================

Transform3DComponent* Transform3DManager::AddComponent(EntityID entity, ID3D12Device* device) {

	// 追加
	components_[entity] = Transform3DComponent();
	components_[entity].Init();
	components_[entity].UpdateMatrix();

	buffers_[entity].CreateConstBuffer(device);

	return &components_[entity];
}

void Transform3DManager::RemoveComponent(EntityID entity) {

	components_.erase(entity);
	buffers_.erase(entity);
}

void Transform3DManager::Update() {

	if (buffers_.empty()) {
		return;
	}

	for (auto& [entityID, buffer] : buffers_) {

		// 行列更新
		components_[entityID].UpdateMatrix();
		// GPU転送
		buffer.TransferData(components_[entityID].matrix);
	}
}

Transform3DComponent* Transform3DManager::GetComponent(EntityID entity) {

	auto it = components_.find(entity);
	return (it != components_.end()) ? &it->second : nullptr;
}

const DxConstBuffer<TransformationMatrix>& Transform3DManager::GetBuffer(EntityID entity) const {

	auto it = buffers_.find(entity);
	if (it != buffers_.end()) {

		return it->second;
	}
	ASSERT(FALSE, "not found transformData");
	return it->second;
}