#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleSpawnModule.h>
#include <Engine/Effect/Particle/Module/Base/ICPUParticleUpdateModule.h>
#include <Engine/Effect/Particle/Module/ParticleModuleRegistry.h>

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

	void Init(Asset* asset, ParticlePrimitiveType primitiveType);

	// 発生処理
	void Emit(std::list<CPUParticle::ParticleData>& particles, float deltaTime);
	// 更新処理
	void Update(CPUParticle::ParticleData& particle, float deltaTime);

	void ImGui();

	// helpers
	// 発生モジュールの選択
	void SetSpawner(ParticleSpawnModuleID id);
	// 更新モジュール
	// 追加
	void AddUpdater(ParticleUpdateModuleID id);
	// 削除
	void RemoveUpdater(uint32_t index);
	// 入れ替え
	void SwapUpdater(uint32_t from, uint32_t to);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	ParticlePrimitiveType primitiveType_;

	// 発生間隔
	float duration_;
	// 現在の経過時間
	float elapsed_;

	using SpawnRegistry = ParticleModuleRegistry<ICPUParticleSpawnModule, ParticleSpawnModuleID>;
	using UpdateRegistry = ParticleModuleRegistry<ICPUParticleUpdateModule, ParticleUpdateModuleID>;

	std::unique_ptr<ICPUParticleSpawnModule>  spawner_;
	std::vector<std::unique_ptr<ICPUParticleUpdateModule>> updaters_;

	// editor
	ParticleSpawnModuleID selectSpawnModule_;
};