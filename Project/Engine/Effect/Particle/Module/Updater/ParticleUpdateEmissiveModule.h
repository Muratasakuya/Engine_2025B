#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	ParticleUpdateEmissiveModule class
//============================================================================
class ParticleUpdateEmissiveModule :
	public ICPUParticleUpdateModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleUpdateEmissiveModule() = default;
	~ParticleUpdateEmissiveModule() = default;

	void Init() override;

	void Execute(CPUParticle::ParticleData& particle, float deltaTime) override;

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	const char* GetName() const override { return "Emissive"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleUpdateModuleID ID = ParticleUpdateModuleID::Emissive;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 発光
	ParticleCommon::LerpValue<float> intencity_; // 発光度合い
	ParticleCommon::LerpValue<Vector3> color_;   // 色

	EasingType easingType_;
};