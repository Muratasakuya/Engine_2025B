#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleModule.h>

//============================================================================
//	ICPUParticleUpdateModule class
//============================================================================
class ICPUParticleUpdateModule :
	public ICPUParticleModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ICPUParticleUpdateModule() = default;
	~ICPUParticleUpdateModule() = default;

	virtual void Execute(CPUParticle::ParticleData& particle, float deltaTime) = 0;
};