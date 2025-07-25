#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleSpawnModule.h>
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>

// c++
#include <memory>
#include <vector>

//============================================================================
//	ParticlePhase class
//============================================================================
class ParticlePhase {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticlePhase() = default;
	~ParticlePhase() = default;

	//--------- functions ----------------------------------------------------

	void Emit(std::list<CPUParticle::ParticleData>& particles);

	void Update(CPUParticle::ParticleData& particle, float deltaTime);

	void ImGui();

	//--------- variables ----------------------------------------------------

	// 発生間隔
	float duration;
	// 現在の経過時間
	float elapsed;

	// 処理を行う関数
	std::unique_ptr<ICPUParticleSpawnModule> spawner;
	std::vector<std::unique_ptr<ICPUParticleUpdateModule>> updaters;

	// 親オブジェクトのID
	std::optional<uint32_t> parentObjectId;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------


};