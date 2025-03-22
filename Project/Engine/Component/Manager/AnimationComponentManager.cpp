#define NOMINMAX

#include "AnimationComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationComponentManager classMethods
//============================================================================

void AnimationComponentManager::AddComponent(EntityID entity, std::any args) {

	auto [animationName, asset] =
		std::any_cast<std::tuple<std::optional<std::string>, Asset*>>(args);

	// animationをしない場合でも要素は追加しておく
	components_.resize(std::max(static_cast<EntityID>(components_.size()), entity + 1));
	if (!animationName.has_value()) {
		return;
	}

	components_[entity].Init(*animationName, asset);
	components_[entity].SetPlayAnimation(*animationName, true);
}

void AnimationComponentManager::RemoveComponent(EntityID entity) {

	// entity削除
	std::swap(components_[entity], components_.back());
	components_.pop_back();
}

void AnimationComponentManager::Update() {

	for (auto& component : components_) {

		// animation更新処理
		component.Update();
	}
}

AnimationComponent* AnimationComponentManager::GetComponent(EntityID entity) {

	return &components_[entity];
}