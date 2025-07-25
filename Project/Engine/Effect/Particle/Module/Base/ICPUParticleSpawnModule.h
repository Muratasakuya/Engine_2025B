#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleModule.h>

// c++
#include <list>

//============================================================================
//	ICPUParticleSpawnModule class
//============================================================================
class ICPUParticleSpawnModule :
	public ICPUParticleModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ICPUParticleSpawnModule() = default;
	virtual ~ICPUParticleSpawnModule() = default;

	virtual void Execute(std::list<CPUParticle::ParticleData>& particles) = 0;
};