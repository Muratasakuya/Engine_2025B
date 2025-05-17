#include "MaterialSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/EntityManager.h>
#include <Engine/Core/Component/MaterialComponent.h>

//============================================================================
//	MaterialSystem classMethods
//============================================================================

Archetype MaterialSystem::Signature() const {

	Archetype arch{};
	arch.set(EntityManager::GetTypeID<MaterialComponent>());
	return arch;
}

void MaterialSystem::Update(EntityManager& entityManager) {

	for (uint32_t entity : entityManager.View(Signature())) {

		auto* materials = entityManager.GetComponent<MaterialComponent, true>(entity);
		for (auto& material : *materials) {

			material.UpdateUVTransform();
		}
	}
}