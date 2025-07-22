#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

// front
class Asset;

//============================================================================
//	GPU: structures
//============================================================================

struct GPUParticleGroup {

	// emitter
	Vector3 emitterRotation;     // 回転(すべて共通)
	ParticleEmitterData emitter; // 各形状

	// buffers
	ParticleEmitterData emitterBuffer;
	ParticleCommon::PrimitiveBufferData primitiveBuffer;
	DxStructuredBuffer<ParticleCommon::TransformForGPU> transformBuffer;
	DxStructuredBuffer<GPUParticle::MaterialForGPU> materialBuffer;
	DxStructuredBuffer<GPUParticle::ParticleForGPU> particleBuffer;
	// freeList
	DxStructuredBuffer<uint32_t> freeListIndexBuffer_;
	DxStructuredBuffer<uint32_t> freeListBuffer_;

	// 描画情報
	BlendMode blendMode;
};

//============================================================================
//	CPU: structures
//============================================================================

struct CPUParticleGroup {

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

	// 描画情報
	BlendMode blendMode;
};

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleSystem() = default;
	~ParticleSystem() = default;

	void Init(ID3D12Device* device, Asset* asset, const std::string& name);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;

	// system名
	std::string name_;

	// GPU
	std::unordered_map<std::string, std::vector<GPUParticleGroup>> gpuParticleGroups_;
	// CPU
	std::unordered_map<std::string, std::vector<CPUParticleGroup>> cpuParticleGroups_;

	//--------- functions ----------------------------------------------------


};