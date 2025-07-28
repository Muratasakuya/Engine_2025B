#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Module/Base/ICPUParticleModule.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

// imgui
#include <imgui.h>
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

	// editor
	void ImGuiRenderParam();
	void ImGuiEmitParam();

	//--------- accessor -----------------------------------------------------

	void SetAsset(Asset* asset) { asset_ = asset; }
	void SetPrimitiveType(ParticlePrimitiveType type);

	// データ共有
	void ShareCommonParam(ICPUParticleSpawnModule* other);
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

	// 移動速度
	ParticleValue<float> moveSpeed_;

	// TextureInfo、ランダムがないのでそのまま渡す
	CPUParticle::TextureInfoForGPU textureInfo_;

	// Primtive
	ParticleCommon::PrimitiveData<false> primitive_;

	// editor
	const Color emitterLineColor_ = Color::Yellow(0.4f);
	std::string textureName_;
	std::string noiseTextureName_;

	//--------- functions ----------------------------------------------------

	// init
	void InitCommonData();

	// helper
	void SetCommonData(CPUParticle::ParticleData& particle);
private:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// editor
	void DragAndDropTexture();
	void ImageButtonWithLabel(const char* id,
		const std::string& label, ImTextureID textureId, const ImVec2& size);
};