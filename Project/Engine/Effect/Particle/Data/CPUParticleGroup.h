#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Data/Base/BaseParticleGroup.h>
#include <Engine/Effect/Particle/Phase/ParticlePhase.h>

//============================================================================
//	CPUParticleGroup class
//============================================================================
class CPUParticleGroup :
	public BaseParticleGroup {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	CPUParticleGroup() = default;
	~CPUParticleGroup() = default;

	void Create(ID3D12Device* device, ParticlePrimitiveType primitiveType);

	void Update();

	void ImGui();

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// インスタンス数
	uint32_t numInstance_;

	// フェーズ
	std::vector<ParticlePhase> phases_;

	// データ
	std::list<CPUParticle::ParticleData> particles_;
	// 転送データ
	std::vector<CPUParticle::MaterialForGPU> transferMaterials_;
	std::vector<CPUParticle::TextureInfoForGPU> transferTextureInfos_;
	std::vector<ParticleCommon::TransformForGPU> transferTransforms_;
	ParticleCommon::PrimitiveData<true> transferPrimitives_;

	// buffers
	DxStructuredBuffer<CPUParticle::MaterialForGPU> materialBuffer_;
	DxStructuredBuffer<CPUParticle::TextureInfoForGPU> textureInfoBuffer_;

	// 描画情報
	BlendMode blendMode_;
	std::string textureName_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdatePhase();
	void UpdateTransferData(uint32_t particleIndex,
		const CPUParticle::ParticleData& particle);
	void TransferBuffer();
};