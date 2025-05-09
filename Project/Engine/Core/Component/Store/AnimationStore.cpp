#define NOMINMAX

#include "AnimationStore.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationStore classMethods
//============================================================================

void AnimationStore::AddComponent(uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [animationName, asset] =
		std::any_cast<std::tuple<std::optional<std::string>, Asset*>>(args);

	// animationをしない場合でも要素は追加しておく
	// component追加
	components_.push_back(std::make_unique<AnimationComponent>());
	components_.back()->Init(*animationName, asset);
	components_.back()->SetPlayAnimation(*animationName, true);
	
	// index設定
	SetEntityIndex(entityId, index);
}

void AnimationStore::RemoveComponent(uint32_t entityId) {

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

void AnimationStore::Update() {

	for (auto& component : components_) {

		// animation更新処理
		component->Update();
	}
}

AnimationComponent* AnimationStore::GetComponent(uint32_t entityId) {

	size_t index = entityToIndex_[entityId];
	return components_[index].get();
}