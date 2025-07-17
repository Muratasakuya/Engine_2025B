#include "TransformSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Core/ObjectPoolManager.h>
#include <Engine/Object/Data/Transform.h>

//============================================================================
//	TransformSystem3D classMethods
//============================================================================

Archetype Transform3DSystem::Signature() const {

	Archetype arch{};
	arch.set(ObjectPoolManager::GetTypeID<Transform3D>());
	return arch;
}

void Transform3DSystem::Update(ObjectPoolManager& ObjectPoolManager) {

	for (uint32_t object : ObjectPoolManager.View(Signature())) {

		auto* transform = ObjectPoolManager.GetData<Transform3D>(object);
		transform->UpdateMatrix();
	}
}

//============================================================================
//	Transform2DSystem classMethods
//============================================================================

Archetype Transform2DSystem::Signature() const {

	Archetype arch{};
	arch.set(ObjectPoolManager::GetTypeID<Transform2D>());
	return arch;
}

void Transform2DSystem::Update(ObjectPoolManager& ObjectPoolManager) {

	for (uint32_t object : ObjectPoolManager.View(Signature())) {

		auto* transform = ObjectPoolManager.GetData<Transform2D>(object);
		transform->UpdateMatrix();
	}
}