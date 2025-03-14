#include "MaterialManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MaterialManager classMethods
//============================================================================

void MaterialManager::AddComponent(EntityID entity, std::any args) {

	auto [meshNum] = std::any_cast<std::tuple<size_t>>(args);

	// component追加
	components_.emplace_back();

	// 追加、ModelのMeshの数に合わせる
	components_[entity].resize(meshNum);
	for (uint32_t index = 0; index < meshNum; ++index) {

		components_[entity][index].Init();
	}
}

void MaterialManager::RemoveComponent(EntityID entity) {

	// component削除
	components_.erase(components_.begin() + entity);
}

Material* MaterialManager::GetComponent(EntityID entity) {

	// 単一のmaterialのみ返す
	// multiMaterialの時はGetComponentList()から取得する
	return &components_[entity].front();
}

std::vector<Material*> MaterialManager::GetComponentList(EntityID entity) {

	// 配列のmaterialを返す
	std::vector<Material*> materials;
	for (Material& mat : components_.at(entity)) {

		materials.emplace_back(&mat);
	}
	return materials;
}