#define NOMINMAX

#include "AnimationComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationComponentManager classMethods
//============================================================================

void AnimationComponentManager::AddComponent(uint32_t entity, std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	auto [animationName, asset] =
		std::any_cast<std::tuple<std::optional<std::string>, Asset*>>(args);

	// animationをしない場合でも要素は追加しておく
	components_.resize(std::max(static_cast<uint32_t>(components_.size()), entity + 1));
	if (!animationName.has_value()) {
		return;
	}

	components_[entity].Init(*animationName, asset);
	components_[entity].SetPlayAnimation(*animationName, true);
}

void AnimationComponentManager::RemoveComponent(uint32_t entity) {

	if (!Algorithm::Find(entityToIndex_, entity)) {
		return;
	}

	size_t index = entityToIndex_.at(entity);
	size_t lastIndex = components_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(components_[index], components_[lastIndex]);

		// 交換されたentityIdを更新
		uint32_t movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	components_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(entity);
}

void AnimationComponentManager::Update() {

	for (auto& component : components_) {

		// animation更新処理
		component.Update();
	}
}

AnimationComponent* AnimationComponentManager::GetComponent(uint32_t entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return &components_.at(index);

	}
	return nullptr;
}