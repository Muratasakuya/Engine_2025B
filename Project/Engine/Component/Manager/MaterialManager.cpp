#include "MaterialManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	MaterialManager classMethods
//============================================================================

std::vector<Material*> MaterialManager::AddComponent(
	EntityID entity, size_t meshNum, ID3D12Device* device) {

	// 追加、ModelのMeshの数に合わせる
	components_[entity].resize(meshNum);
	buffers_[entity].resize(meshNum);
	for (uint32_t index = 0; index < meshNum; ++index) {

		components_[entity][index].Init();
		buffers_[entity][index].CreateConstBuffer(device);
	}

	std::vector<Material*> result;
	for (Material& mat : components_[entity]) {

		result.emplace_back(&mat);
	}
	return result;
}

void MaterialManager::RemoveComponent(EntityID entity) {

	components_.erase(entity);
	buffers_.erase(entity);
}

void MaterialManager::Update() {

	if (buffers_.empty()) {
		return;
	}

	for (auto& [entityID, buffer] : buffers_) {
		for (uint32_t index = 0; index < buffer.size(); ++index) {

			buffer[index].TransferData(components_[entityID][index]);
		}
	}
}

std::vector<Material*> MaterialManager::GetComponent(EntityID entity) {

	auto it = components_.find(entity);
	if (it == components_.end()) {
		return {};
	}

	std::vector<Material*> result;
	for (Material& mat : it->second) {

		result.emplace_back(&mat);
	}
	return result;
}

const std::vector<DxConstBuffer<Material>>& MaterialManager::GetBuffer(EntityID entity) const {

	auto it = buffers_.find(entity);
	if (it != buffers_.end()) {

		return it->second;
	}
	ASSERT(FALSE, "not found materialData");
	return it->second;
}