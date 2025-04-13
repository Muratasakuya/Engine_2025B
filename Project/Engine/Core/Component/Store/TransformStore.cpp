#define NOMINMAX

#include "TransformStore.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	TransformStore classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void Transform3DStore::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.emplace_back();
	components_.back().Init();
	components_.back().UpdateMatrix();

	// index設定
	SetEntityIndex(entityId, index);
}

void Transform3DStore::RemoveComponent(uint32_t entityId) {

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

void Transform3DStore::Update() {

	for (auto& component : components_) {

		// 行列更新
		component.UpdateMatrix();
	}
}

Transform3DComponent* Transform3DStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return &components_[index];
}

//============================================================================
// 2D
//============================================================================

void Transform2DStore::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.push_back(std::make_unique<Transform2DComponent>());
	components_.back()->Init();
	components_.back()->UpdateMatrix();

	// index設定
	SetEntityIndex(entityId, index);
}

void Transform2DStore::RemoveComponent(uint32_t entityId) {

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

void Transform2DStore::Update() {

	for (auto& component : components_) {

		// 行列更新
		component->UpdateMatrix();
	}
}

Transform2DComponent* Transform2DStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return components_[index].get();
}