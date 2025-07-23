#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

//============================================================================
//	GPUParticleGroup class
//============================================================================
class GPUParticleGroup {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GPUParticleGroup() = default;
	~GPUParticleGroup() = default;

	void Create(ID3D12Device* device, ParticlePrimitiveType primitiveType);

	void Update();

	void ImGui(ID3D12Device* device);

	//--------- accessor -----------------------------------------------------

	ParticlePrimitiveType GetPrimitiveType() const { return primitiveBuffer_.type; }
	ParticleEmitterShape GetEmitterShape() const { return emitter_.shape; }
	BlendMode GetBlendMode() const { return blendMode_; }
	const std::string& GetTextureName() const { return textureName_; }

	D3D12_GPU_VIRTUAL_ADDRESS GetPrimitiveBufferAdress() const;
	D3D12_GPU_VIRTUAL_ADDRESS GetEmitterShapeBufferAdress() const;
	const DxConstBuffer<ParticleEmitterCommon>& GetEmitterCommonBuffer() const { return emitterBuffer_.common; }
	const DxStructuredBuffer<ParticleCommon::TransformForGPU>& GetTransformBuffer() const { return transformBuffer_; }
	const DxStructuredBuffer<GPUParticle::MaterialForGPU>& GetMaterialBuffer() const { return materialBuffer_; }
	const DxStructuredBuffer<GPUParticle::ParticleForGPU>& GetParticleBuffer() const { return particleBuffer_; }
	const DxStructuredBuffer<uint32_t>& GetFreeListIndexBuffer() const { return freeListIndexBuffer_; }
	const DxStructuredBuffer<uint32_t>& GetFreeListBuffer() const { return freeListBuffer_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// emitter
	Vector3 emitterRotation_;     // 回転(すべて共通)
	ParticleEmitterData emitter_; // 各形状

	// 時間管理
	float frequency_;
	float frequencyTime_;

	// buffers
	ParticleEmitterBufferData emitterBuffer_;
	ParticleCommon::PrimitiveBufferData primitiveBuffer_;
	DxStructuredBuffer<ParticleCommon::TransformForGPU> transformBuffer_;
	DxStructuredBuffer<GPUParticle::MaterialForGPU> materialBuffer_;
	DxStructuredBuffer<GPUParticle::ParticleForGPU> particleBuffer_;
	// freeList
	DxStructuredBuffer<uint32_t> freeListIndexBuffer_;
	DxStructuredBuffer<uint32_t> freeListBuffer_;

	// 描画情報
	BlendMode blendMode_;
	std::string textureName_;

	//--------- functions ----------------------------------------------------

	// create
	void CreatePrimitiveBuffer(ID3D12Device* device, ParticlePrimitiveType primitiveType);

	// update
	void UpdateEmitter();

	// editor
	void SelectEmitter(ID3D12Device* device);
	void EditEmitter();
};