#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

//============================================================================
//	CPUParticleGroup class
//============================================================================
class CPUParticleGroup {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	CPUParticleGroup() = default;
	~CPUParticleGroup() = default;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// emitter
	Vector3 emitterRotation;     // 回転(すべて共通)
	ParticleEmitterData emitter; // 各形状

	// インスタンス数
	uint32_t numInstance;

	// データ
	std::list<CPUParticle::ParticleData> particles;
	// 転送データ
	std::vector<CPUParticle::MaterialForGPU> transferMaterials;
	std::vector<CPUParticle::TextureInfoForGPU> transferTextureInfos;
	std::vector<ParticleCommon::TransformForGPU> transferTransforms;
	std::vector<ParticleCommon::PrimitiveData> transferPrimitives;

	// buffers
	ParticleCommon::PrimitiveBufferData primitiveBuffer;
	DxStructuredBuffer<ParticleCommon::TransformForGPU> transformBuffer;
	DxStructuredBuffer<CPUParticle::MaterialForGPU> materialBuffer;
	DxStructuredBuffer<CPUParticle::TextureInfoForGPU> textureInfoBuffer;

	//--------- functions ----------------------------------------------------

};