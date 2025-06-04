#include "TransformSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/ECS/Components/TransformComponent.h>

//============================================================================
//	TransformSystem3D classMethods
//============================================================================

Archetype Transform3DSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<Transform3DComponent>());
	return arch;
}

void Transform3DSystem::Update(EntityManager& entityManager) {

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* transform = entityManager.GetComponent<Transform3DComponent>(entity);
		transform->UpdateMatrix();
	}
}

//============================================================================
//	Transform2DSystem classMethods
//============================================================================

Archetype Transform2DSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<Transform2DComponent>());
	return arch;
}

void Transform2DSystem::Update(EntityManager& entityManager) {

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* transform = entityManager.GetComponent<Transform2DComponent>(entity);
		transform->UpdateMatrix();
	}
}