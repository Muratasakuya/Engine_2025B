#include "SpriteComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/TransformComponent.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	SpriteComponentManager classMethods
//============================================================================

void SpriteComponentManager::AddComponent([[maybe_unused]] EntityID entity, std::any args) {

	size_t index = components_.size();

	entityToIndex_[entity] = index;
	indexToEntity_.emplace_back(entity);

	auto [textureName, transform, device, asset] =
		std::any_cast<std::tuple<std::string, Transform2DComponent*, ID3D12Device*, Asset*>>(args);

	// component追加
	components_.push_back(std::make_unique<SpriteComponent>(device, asset, textureName, *transform));
	// transform追加
	transforms_.push_back(transform);
}

void SpriteComponentManager::RemoveComponent(EntityID entity) {

	size_t index = entityToIndex_.at(entity);
	size_t lastIndex = components_.size() - 1;

	if (index != lastIndex) {

		// 末尾と交換
		std::swap(components_[index], components_[lastIndex]);
		std::swap(transforms_[index], transforms_[lastIndex]);

		// 交換されたentityIdを更新
		EntityID movedEntityId = indexToEntity_[lastIndex];
		entityToIndex_[movedEntityId] = index;
		indexToEntity_[index] = movedEntityId;
	}

	// 末尾を削除
	components_.pop_back();
	transforms_.pop_back();
	indexToEntity_.pop_back();
	entityToIndex_.erase(entity);
}

void SpriteComponentManager::Update() {

	for (uint32_t index = 0; index < components_.size(); ++index) {

		components_[index]->UpdateVertex(*transforms_[index]);
	}
}

SpriteComponent* SpriteComponentManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(entityToIndex_, entity)) {

		size_t index = entityToIndex_.at(entity);
		return components_.at(index).get();

	}
	return nullptr;
}