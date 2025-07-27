#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleModule.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

// c++
#include <list>
// front
class Asset;

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

	virtual void UpdateEmitter() {}
	virtual void DrawEmitter() {}

	//--------- accessor -----------------------------------------------------

	void SetAsset(Asset* asset) { asset_ = asset; }
	void SetPrimitiveType(ParticlePrimitiveType type);
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// 全ての発生に置いて共通しているパラメータ
	// Emit
	ParticleValue<uint32_t> emitCount_; // 発生数
	ParticleValue<float> lifeTime_;     // 生存時間

	// Material
	ParticleCommon::EditLerpValue<Color> color_; // 色

	// TextureInfo、ランダムがないのでそのまま渡す
	CPUParticle::TextureInfoForGPU textureInfo_;

	// Transform
	ParticleBillboardType billboardType_;          // ビルボードの種類
	ParticleCommon::EditLerpValue<Vector3> scale_; // 拡縮
	ParticleValue<float> moveSpeed_;               // 移動速度

	// Primtive
	ParticleCommon::PrimitiveData<false> primitive_;

	// editor
	const Color emitterLineColor = Color::Yellow(0.4f);

	//--------- functions ----------------------------------------------------

	// init
	void InitCommonData();

	// editor
	void ImGuiCommon();

	// helper
	void SetCommonData(CPUParticle::ParticleData& particle);
};