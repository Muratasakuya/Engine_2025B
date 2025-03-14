#include "Transform3DManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	Transform3DManager classMethods
//============================================================================

void Transform3DManager::AddComponent(EntityID entity, [[maybe_unused]] std::any args) {

	// component追加
	components_.emplace_back();

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