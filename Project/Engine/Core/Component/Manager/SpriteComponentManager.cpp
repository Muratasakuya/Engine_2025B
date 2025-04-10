#include "SpriteComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	SpriteComponentManager classMethods
//============================================================================

void SpriteComponentManager::AddComponent([[maybe_unused]] uint32_t entityId, std::any args) {

	size_t index = components_.size();

	auto [textureName, transform, device, asset] =
		std::any_cast<std::tuple<std::string, Transform2DComponent*, ID3D12Device*, Asset*>>(args);

	// component追加
	components_.push_back(std::make_unique<SpriteComponent>(device, asset, textureName, *transform));
	// transform追加
	transforms_.push_back(transform);

	// index設定
	SetEntityIndex(entityId, index);
}

void SpriteComponentManager::RemoveComponent(uint32_t entityId) {

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

void SpriteComponentManager::Update() {

	for (uint32_t index = 0; index < components_.size(); ++index) {

		components_[index]->UpdateVertex(*transforms_[index]);
	}
}

SpriteComponent* SpriteComponentManager::GetComponent(uint32_t entityId) {

	if (Algorithm::Find(entityToIndex_, entityId)) {

		size_t index = entityToIndex_.at(entityId);
		return components_.at(index).get();

	}
	return nullptr;
}