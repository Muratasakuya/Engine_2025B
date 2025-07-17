#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Core/ObjectPool.h>

// front
class ObjectPoolManager;

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

	virtual void Update(ObjectPoolManager& ObjectPoolManager) = 0;
};