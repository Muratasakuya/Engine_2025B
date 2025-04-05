#define NOMINMAX

#include "MaterialManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MaterialManager classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void MaterialManager::AddComponent(EntityID entity, std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	auto [modelData, asset] =
		std::any_cast<std::tuple<ModelData, Asset*>>(args);

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加、ModelのMeshの数に合わせる
	components_[entity].resize(modelData.meshes.size());
	for (uint32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex) {

		components_[entity][meshIndex].Init();
		components_[entity][meshIndex].textureIndex =
			asset->GetTextureGPUIndex(modelData.meshes[meshIndex].textureName.value_or("white"));
	}
}

void MaterialManager::RemoveComponent(EntityID entity) {

	if (!Algorithm::Find(entityToIndex_, entity)) {
		return;
	}

	size_t index = entityToIndex_.at(entity);
	size_t lastIndex = components_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(components_[index], components_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	components_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(entity);
}

Material* MaterialManager::GetComponent(EntityID entity) {

	// 単一のmaterialのみ返す
	// multiMaterialの時はGetComponentList()から取得する
	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return &components_.at(index).front();

	}
	return nullptr;
}

std::vector<Material*> MaterialManager::GetComponentList(EntityID entity) {

	// 配列のmaterialを返す
	std::vector<Material*> materials;

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		for (Material& mat : components_.at(index)) {

			materials.emplace_back(&mat);
		}
	}
	return materials;
}

//============================================================================
// 2D
//============================================================================

void SpriteMaterialManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = SpriteMaterial();
	components_[entity].Init();
}

void SpriteMaterialManager::RemoveComponent(EntityID entity) {

	if (!Algorithm::Find(entityToIndex_, entity)) {
		return;
	}

	size_t index = entityToIndex_.at(entity);
	size_t lastIndex = components_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(components_[index], components_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	components_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(entity);
}

SpriteMaterial* SpriteMaterialManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return &components_.at(index);

	}
	return nullptr;
}
