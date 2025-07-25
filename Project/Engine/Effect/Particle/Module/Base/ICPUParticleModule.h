#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>

//============================================================================
//	eum class
//============================================================================

// 処理に必要な値
enum class ParticleModuleFlags {

	NeedVelocity,
	NeedRotation,
	NeedParent,
};

//============================================================================
//	ICPUParticleModule class
//============================================================================
class ICPUParticleModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ICPUParticleModule() = default;
	virtual ~ICPUParticleModule() = default;

	virtual void ImGui() = 0;

	//--------- accessor -----------------------------------------------------

	virtual const char* GetName() const = 0;
	virtual ParticleModuleFlags GetFlags() const = 0;
};