#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	ParticleUpdatePrimitiveModule class
//============================================================================
class ParticleUpdatePrimitiveModule :
	public ICPUParticleUpdateModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleUpdatePrimitiveModule() = default;
	~ParticleUpdatePrimitiveModule() = default;

	void Init() override;

	void Execute(CPUParticle::ParticleData& particle, float deltaTime) override;

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	const char* GetName() const override { return "Primitive"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleUpdateModuleID ID = ParticleUpdateModuleID::Primitive;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 形状、particleの型によって処理を変更
	ParticlePrimitiveType type_;
	ParticleCommon::LerpValue<ParticleCommon::PrimitiveData<false>> primitive_;

	EasingType easingType_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdatePlane(CPUParticle::ParticleData& particle);
	void UpdateRing(CPUParticle::ParticleData& particle);
	void UpdateCylinder(CPUParticle::ParticleData& particle);
};