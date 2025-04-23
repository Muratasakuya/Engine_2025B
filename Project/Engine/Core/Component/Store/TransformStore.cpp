#define NOMINMAX

#include "TransformStore.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>
#include <Engine/Scene/Camera/CameraManager.h>

//============================================================================
//	TransformStore classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void Transform3DStore::AddComponent(uint32_t entityId, [[maybe_unused]] std::any args) {

	size_t index = components_.size();

	// component追加
	components_.push_back(std::make_unique<Transform3DComponent>());
	components_.back()->Init();
	components_.back()->UpdateMatrix();

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
		component->UpdateMatrix();
	}
}

Transform3DComponent* Transform3DStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return components_[index].get();
}

//============================================================================
// Effect
//============================================================================

void EffectTransformStore::Init(CameraManager* cameraManager) {

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;
}

void EffectTransformStore::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	// component追加
	components_.push_back(std::make_unique<EffectTransformComponent>());
	components_.back()->Init();

	// index設定
	SetEntityIndex(entityId, index);
}

void EffectTransformStore::RemoveComponent(uint32_t entityId) {

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

void EffectTransformStore::Update() {

	Matrix4x4 billboardMatrix = cameraManager_->GetCamera()->GetBillboardMatrixMatrix();
	for (auto& component : components_) {

		// 行列更新
		component->UpdateMatrix(billboardMatrix);
	}
}

EffectTransformComponent* EffectTransformStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return components_[index].get();
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