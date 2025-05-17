#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/System/Base/ISystem.h>


//============================================================================
//	MaterialSystem class
//============================================================================
class MaterialSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialSystem() = default;
	~MaterialSystem() = default;

	Archetype Signature() const override;

	void Update(EntityManager& entityManager) override;
};