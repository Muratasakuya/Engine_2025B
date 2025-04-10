#define NOMINMAX

#include "TransformManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	TransformManager classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void Transform3DManager::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.emplace_back();
	components_.back().Init();
	components_.back().UpdateMatrix();

	// index設定
	SetEntityIndex(entityId, index);
}

void Transform3DManager::RemoveComponent(uint32_t entityId) {

	// 見つかなければ削除しない
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

void Transform3DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component.UpdateMatrix();
	}
}

Transform3DComponent* Transform3DManager::GetComponent(uint32_t entityId) {

	if (Algorithm::Find(entityToIndex_, entityId)) {

		size_t index = entityToIndex_.at(entityId);
		return &components_.at(index);

	}
	return nullptr;
}

//============================================================================
// 2D
//============================================================================

void Transform2DManager::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.push_back(std::make_unique<Transform2DComponent>());
	components_.back()->Init();
	components_.back()->UpdateMatrix();

	// index設定
	SetEntityIndex(entityId, index);
}

void Transform2DManager::RemoveComponent(uint32_t entityId) {

	// 見つかなければ削除しない
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

void Transform2DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component->UpdateMatrix();
	}
}

Transform2DComponent* Transform2DManager::GetComponent(uint32_t entityId) {

	if (Algorithm::Find(entityToIndex_, entityId)) {

		size_t index = entityToIndex_.at(entityId);
		return components_.at(index).get();

	}
	return nullptr;
}