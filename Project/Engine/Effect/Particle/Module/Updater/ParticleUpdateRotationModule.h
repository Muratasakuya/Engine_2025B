#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>

//============================================================================
//	ParticleUpdateRotationModule class
//============================================================================
class ParticleUpdateRotationModule :
	public ICPUParticleUpdateModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleUpdateRotationModule() = default;
	~ParticleUpdateRotationModule() = default;

	void Execute(CPUParticle::ParticleData& particle, float deltaTime) override;

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	const char* GetName() const override { return "Rotation"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleUpdateModuleID ID = ParticleUpdateModuleID::Rotation;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ---------------------------------------------------
	
	ParticleCommon::LerpValue<Vector3> rotation_;

	// ビルボードの種類
	ParticleBillboardType billboardType_;

	EasingType easing;
};