#include "SpriteComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/TransformComponent.h>

//============================================================================
//	SpriteComponentManager classMethods
//============================================================================

void SpriteComponentManager::AddComponent([[maybe_unused]] EntityID entity, std::any args) {

	auto [textureName, transform, device, asset] =
		std::any_cast<std::tuple<std::string, Transform2DComponent*, ID3D12Device*, Asset*>>(args);

	// component追加
	components_.emplace_back(SpriteComponent(device, asset, textureName, *transform));
	// transform追加
	transforms_.push_back(*transform);
}

void SpriteComponentManager::RemoveComponent(EntityID entity) {

	components_.erase(components_.begin() + entity);
	transforms_.erase(transforms_.begin() + entity);
}

void SpriteComponentManager::Update() {

	for (uint32_t index = 0; index < components_.size(); ++index) {

		components_[index].UpdateVertex(transforms_[index]);
	}
}

SpriteComponent* SpriteComponentManager::GetComponent(EntityID entity) {

	return &components_[entity];
}