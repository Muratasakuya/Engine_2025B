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

	if (!animationName.has_value()) {
		return;
	}

	components_[entity].Init(*animationName, asset);
	components_[entity].SetPlayAnimation(*animationName, true);
}

void AnimationComponentManager::RemoveComponent(EntityID entity) {

	if (Algorithm::Find(components_, entity)) {

		components_.erase(entity);
	}
}

void AnimationComponentManager::Update() {

	for (auto& [entityID, component] : components_) {

		// animation更新処理
		component.Update();
	}
}

AnimationComponent* AnimationComponentManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(components_, entity, true)) {

		return &components_[entity];
	}
	return nullptr;
}