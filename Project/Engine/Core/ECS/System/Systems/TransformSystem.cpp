#include "TransformSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/Component/TransformComponent.h>

//============================================================================
//	TransformSystem classMethods
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