#include "SpriteBufferSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>

//============================================================================
//	SpriteBufferSystem classMethods
//============================================================================

Archetype SpriteBufferSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<Transform2DComponent>());
	arch.set(EntityManager::GetTypeID<SpriteMaterialComponent>());
	arch.set(EntityManager::GetTypeID<SpriteComponent>());
	return arch;
}

void SpriteBufferSystem::Update(EntityManager& entityManager) {

	// データクリア
	spriteDataMap_.clear();

	const auto& view = entityManager.View(Signature());

	for (const auto& entity : view) {

		auto* transform = entityManager.GetComponent<Transform2DComponent>(entity);
		auto* material = entityManager.GetComponent<SpriteMaterialComponent>(entity);
		auto* sprite = entityManager.GetComponent<SpriteComponent>(entity);

		// spriteの更新処理
		sprite->UpdateVertex(*transform);

		// mapに追加
		spriteDataMap_[sprite->GetLayer()].push_back(SpriteData(transform, material, sprite));
	}
}