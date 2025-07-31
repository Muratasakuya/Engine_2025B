#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Data/Base/BaseParticleGroup.h>

//============================================================================
//	GPUParticleGroup class
//============================================================================
class GPUParticleGroup :
	public BaseParticleGroup {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structures ---------------------------------------------------

	// GPUに渡す親の情報
	struct ParentForGPU {

		Matrix4x4 parentMatrix;
		uint32_t aliveParent;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GPUParticleGroup() = default;
	~GPUParticleGroup() = default;

	void Create(ID3D12Device* device, ParticlePrimitiveType primitiveType);

	void Update();

	// editor
	void ImGui(ID3D12Device* device);

	// json
	Json ToJson() const;
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	void SetIsInitialized(bool isInitialized) { isInitialized_ = isInitialized; }
	bool IsInitialized() const { return isInitialized_; }

	ParticlePrimitiveType GetPrimitiveType() const { return primitiveBuffer_.type; }
	ParticleEmitterShape GetEmitterShape() const { return emitter_.shape; }
	BlendMode GetBlendMode() const { return blendMode_; }
	const std::string& GetTextureName() const { return textureName_; }
	uint32_t GetEmitCount() const { return emitter_.common.count; }

	D3D12_GPU_VIRTUAL_ADDRESS GetEmitterShapeBufferAdress() const;
	const DxConstBuffer<ParentForGPU>& GetParentBuffer() const { return parentBuffer_; }
	const DxConstBuffer<ParticleEmitterCommon>& GetEmitterCommonBuffer() const { return emitterBuffer_.common; }
	const DxStructuredBuffer<ParticleCommon::TransformForGPU>& GetTransformBuffer() const { return transformBuffer_; }
	const DxStructuredBuffer<GPUParticle::MaterialForGPU>& GetMaterialBuffer() const { return materialBuffer_; }
	const DxStructuredBuffer<GPUParticle::ParticleForGPU>& GetParticleBuffer() const { return particleBuffer_; }
	const DxStructuredBuffer<uint32_t>& GetFreeListIndexBuffer() const { return freeListIndexBuffer_; }
	const DxStructuredBuffer<uint32_t>& GetFreeListBuffer() const { return freeListBuffer_; }

	//---------- runtime -----------------------------------------------------

	//----------- emit -------------------------------------------------------

	// 一定間隔
	void FrequencyEmit();
	// 強制発生
	void Emit();

	//---------- setter ------------------------------------------------------

	void SetTransform(const Matrix4x4& matrix);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 時間管理
	float frequency_;
	float frequencyTime_;
	// 初期化したかどうか
	bool isInitialized_;

	// buffers
	ParticleEmitterBufferData emitterBuffer_;
	DxConstBuffer<ParentForGPU> parentBuffer_;
	DxStructuredBuffer<GPUParticle::MaterialForGPU> materialBuffer_;
	DxStructuredBuffer<GPUParticle::ParticleForGPU> particleBuffer_;
	// freeList
	DxStructuredBuffer<uint32_t> freeListIndexBuffer_;
	DxStructuredBuffer<uint32_t> freeListBuffer_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdateEmitter();
	void UpdateParent();

	// editor
	void SelectEmitter(ID3D12Device* device);
};