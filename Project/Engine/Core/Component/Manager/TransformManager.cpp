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

void Transform3DManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = Transform3DComponent();
	components_[entity].Init();
	components_[entity].UpdateMatrix();
}

void Transform3DManager::RemoveComponent(EntityID entity) {

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

void Transform3DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component.UpdateMatrix();
	}
}

Transform3DComponent* Transform3DManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return &components_.at(index);

	}
	return nullptr;
}

//============================================================================
// 2D
//============================================================================

void Transform2DManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = std::make_unique<Transform2DComponent>();
	components_[entity]->Init();
	components_[entity]->UpdateMatrix();
}

void Transform2DManager::RemoveComponent(EntityID entity) {

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

void Transform2DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component->UpdateMatrix();
	}
}

Transform2DComponent* Transform2DManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return components_.at(index).get();

	}
	return nullptr;
}