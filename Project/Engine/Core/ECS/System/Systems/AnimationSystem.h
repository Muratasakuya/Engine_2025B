#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/System/Base/ISystem.h>

//============================================================================
//	AnimationSystem class
//============================================================================
class AnimationSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationSystem() = default;
	~AnimationSystem() = default;

	Archetype Signature() const override;

	void Update(EntityManager& entityManager) override;
};