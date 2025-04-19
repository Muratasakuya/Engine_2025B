#define NOMINMAX

#include "MaterialStore.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MaterialStore classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void MaterialStore::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [modelData, asset] =
		std::any_cast<std::tuple<ModelData, Asset*>>(args);

	// component追加
	components_.emplace_back();

	// 追加、modelのmeshの数に合わせる
	components_.back().resize(modelData.meshes.size());
	for (uint32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex) {

		components_.back()[meshIndex].Init();
		components_.back()[meshIndex].material.textureIndex =
			asset->GetTextureGPUIndex(modelData.meshes[meshIndex].textureName.value_or("white"));

		// normalMap用のTextureがあれば設定する
		if (modelData.meshes[meshIndex].normalMapTexture.has_value()) {

			components_.back()[meshIndex].material.normalMapTextureIndex =
				asset->GetTextureGPUIndex(modelData.meshes[meshIndex].normalMapTexture.value());
			components_.back()[meshIndex].material.enableNormalMap = true;
		}
	}

	// index設定
	SetEntityIndex(entityId, index);
}

void MaterialStore::RemoveComponent(uint32_t entityId) {

	// 見つからなければ削除しない
	if (!Algorithm::Find(entityToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = entityToIndex_.at(entityId);
	size_t lastIndex = components_.size() - 1;

	// entityIndex削除
	SwapToPopbackIndex(entityId, lastIndex);

	// component削除
	std::swap(components_[index], components_[lastIndex]);
	components_.pop_back();
}

void MaterialStore::Update() {

	for (auto& component : components_) {

		// uvTransform更新
		for (uint32_t meshIndex = 0; meshIndex < component.size(); ++meshIndex) {

			component[meshIndex].UpdateUVTransform();
		}
	}
}

MaterialComponent* MaterialStore::GetComponent(uint32_t entityId) {

	// 単一のmaterialのみ返す
	// multiMaterialの時はGetComponentList()から取得する
	size_t index = entityToIndex_[entityId];
	return &components_[index].front();
}

std::vector<MaterialComponent*> MaterialStore::GetComponentList(uint32_t entityId) {

	// 配列のmaterialを返す
	std::vector<MaterialComponent*> materials;

	size_t index = entityToIndex_[entityId];
	for (MaterialComponent& mat : components_[index]) {

		materials.emplace_back(&mat);
	}

	return materials;
}

//============================================================================
// Effect
//============================================================================

void EffectMaterialStore::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [textureName, asset] =
		std::any_cast<std::tuple<std::string, Asset*>>(args);

	// component追加
	components_.emplace_back();
	components_.back().Init();
	components_.back().material.textureIndex =
		asset->GetTextureGPUIndex(textureName);

	// index設定
	SetEntityIndex(entityId, index);
}

void EffectMaterialStore::RemoveComponent(uint32_t entityId) {

	// 見つからなければ削除しない
	if (!Algorithm::Find(entityToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = entityToIndex_.at(entityId);
	size_t lastIndex = components_.size() - 1;

	// entityIndex削除
	SwapToPopbackIndex(entityId, lastIndex);

	// component削除
	swap(components_[index], components_[lastIndex]);
	components_.pop_back();
}

void EffectMaterialStore::Update() {

	for (auto& component : components_) {

		// uvTransform更新
		component.UpdateUVTransform();
	}
}

EffectMaterialComponent* EffectMaterialStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return &components_[index];
}

//============================================================================
// 2D
//============================================================================

void SpriteMaterialStore::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.emplace_back();
	components_.back().Init();

	// index設定
	SetEntityIndex(entityId, index);
}

void SpriteMaterialStore::RemoveComponent(uint32_t entityId) {

	// 見つからなければ削除しない
	if (!Algorithm::Find(entityToIndex_, entityId)) {
		return;
	}

	// indexを末尾と交換して削除
	size_t index = entityToIndex_.at(entityId);
	size_t lastIndex = components_.size() - 1;

	// entityIndex削除
	SwapToPopbackIndex(entityId, lastIndex);

	// component削除
	std::swap(components_[index], components_[lastIndex]);
	components_.pop_back();
}

SpriteMaterial* SpriteMaterialStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return &components_[index];
}