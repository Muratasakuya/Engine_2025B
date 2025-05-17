#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/ComponentPool.h>

// front
class EntityManager;

//============================================================================
//	ISystem class
//============================================================================
class ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ISystem() = default;

	virtual ~ISystem() = default;

	virtual Archetype Signature() const = 0;

	virtual void Update(EntityManager& entityManager) = 0;
};