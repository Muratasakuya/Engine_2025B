#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleModule.h>
#include <Engine/Effect/Particle/Module/Base/ParticleLoopableModule.h>

//============================================================================
//	ICPUParticleUpdateModule class
//============================================================================
class ICPUParticleUpdateModule :
	public ICPUParticleModule,
	public ParticleLoopableModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ICPUParticleUpdateModule() = default;
	~ICPUParticleUpdateModule() = default;

	void Init() override {}

	virtual void Execute(CPUParticle::ParticleData& particle, float deltaTime) = 0;
};