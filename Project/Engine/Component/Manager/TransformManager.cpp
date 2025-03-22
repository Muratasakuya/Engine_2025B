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

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = Transform3DComponent();
	components_[entity].Init();
	components_[entity].UpdateMatrix();
}

void Transform3DManager::RemoveComponent(EntityID entity) {

	// component削除
	components_.erase(components_.begin() + entity);
}

void Transform3DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component.UpdateMatrix();
	}
}

Transform3DComponent* Transform3DManager::GetComponent(EntityID entity) {

	return &components_[entity];
}

//============================================================================
// 2D
//============================================================================

void Transform2DManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	// component追加
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));

	// 追加
	components_[entity] = std::make_unique<Transform2DComponent>();
	components_[entity]->Init();
	components_[entity]->UpdateMatrix();
}

void Transform2DManager::RemoveComponent(EntityID entity) {

	// component削除
	components_.erase(components_.begin() + entity);
}

void Transform2DManager::Update() {

	for (auto& component : components_) {

		// 行列更新
		component->UpdateMatrix();
	}
}

Transform2DComponent* Transform2DManager::GetComponent(EntityID entity) {

	return components_[entity].get();
}