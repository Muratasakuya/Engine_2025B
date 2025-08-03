#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleSpawnModule.h>

//============================================================================
//	ParticleSpawnBoxModule class
//============================================================================
class ParticleSpawnBoxModule :
	public ICPUParticleSpawnModule {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleSpawnBoxModule() = default;
	~ParticleSpawnBoxModule() = default;

	void Init() override;

	void Execute(std::list<CPUParticle::ParticleData>& particles) override;

	void UpdateEmitter() override;
	void DrawEmitter() override;

	void ImGui() override;

	// json
	Json ToJson() override;
	void FromJson(const Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool SetCommand(const ParticleCommand& command) override;

	const char* GetName() const override { return "SpawnBox"; }

	//-------- registryID ----------------------------------------------------

	static constexpr ParticleSpawnModuleID ID = ParticleSpawnModuleID::Box;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 emitterRotation_;
	ParticleEmitterBox emitter_;

	//--------- functions ----------------------------------------------------

	Vector3 GetRandomPoint() const;
};