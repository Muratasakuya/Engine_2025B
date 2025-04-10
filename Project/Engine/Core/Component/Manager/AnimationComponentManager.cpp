#define NOMINMAX

#include "AnimationComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationComponentManager classMethods
//============================================================================

void AnimationComponentManager::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [animationName, asset] =
		std::any_cast<std::tuple<std::optional<std::string>, Asset*>>(args);

	// animationをしない場合でも要素は追加しておく
	// component追加
	components_.emplace_back();
	components_.back().Init(*animationName, asset);
	components_.back().SetPlayAnimation(*animationName, true);
	
	// index設定
	SetEntityIndex(entityId, index);
}

void AnimationComponentManager::RemoveComponent(uint32_t entityId) {

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

void AnimationComponentManager::Update() {

	for (auto& component : components_) {

		// animation更新処理
		component.Update();
	}
}

AnimationComponent* AnimationComponentManager::GetComponent(uint32_t entityId) {

	if (Algorithm::Find(entityToIndex_, entityId)) {

		size_t index = entityToIndex_.at(entityId);
		return &components_.at(index);

	}
	return nullptr;
}