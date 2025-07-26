#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

//============================================================================
//	BaseParticleGroup class
//============================================================================
class BaseParticleGroup {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseParticleGroup() = default;
	virtual ~BaseParticleGroup() = default;

	//--------- accessor -----------------------------------------------------

	D3D12_GPU_VIRTUAL_ADDRESS GetPrimitiveBufferAdress() const;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// emitter
	Vector3 emitterRotation_;     // 回転(すべて共通)
	ParticleEmitterData emitter_; // 各形状

	// commonBuffers
	ParticleCommon::PrimitiveBufferData primitiveBuffer_;
	DxStructuredBuffer<ParticleCommon::TransformForGPU> transformBuffer_;

	// 描画情報
	BlendMode blendMode_;
	std::string textureName_;

	//--------- functions ----------------------------------------------------

	// create
	void CreatePrimitiveBuffer(ID3D12Device* device, ParticlePrimitiveType primitiveType);

	// emitter
	void DrawEmitter();
	void EditEmitter();
};