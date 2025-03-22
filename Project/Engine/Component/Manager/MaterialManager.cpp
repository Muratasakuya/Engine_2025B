#define NOMINMAX

#include "MaterialManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MaterialManager classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void MaterialManager::AddComponent(EntityID entity, std::any args) {

	auto [meshNum] = std::any_cast<std::tuple<size_t>>(args);

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

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

//============================================================================
// 2D
//============================================================================

void SpriteMaterialManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = SpriteMaterial();
	components_[entity].Init();
}

void SpriteMaterialManager::RemoveComponent(EntityID entity) {

	components_.erase(components_.begin() + entity);
}

SpriteMaterial* SpriteMaterialManager::GetComponent(EntityID entity) {

	return &components_[entity];
}
