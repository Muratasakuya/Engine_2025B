#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Effect/Particle/Structures/ParticleEmitterStructures.h>

// c++
#include <array>
#include <cstdint>
// front
class SRVDescriptor;
class DxShaderCompiler;
class DxCommand;

//============================================================================
//	GPUParticleUpdater class
//============================================================================
class GPUParticleUpdater {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GPUParticleUpdater() = default;
	~GPUParticleUpdater() = default;

	void Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
		DxShaderCompiler* shaderCompiler);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static const uint32_t kEmitterShapeCount = static_cast<uint32_t>(ParticleEmitterShape::Count);

	// 初期化
	std::unique_ptr<PipelineState> initPipeline_;
	// 発生
	std::array<std::unique_ptr<PipelineState>, kEmitterShapeCount> emitPipelines_;
	// 更新
	std::unique_ptr<PipelineState> updatePipeline_;

	//--------- functions ----------------------------------------------------

	void InitPipelines(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
		DxShaderCompiler* shaderCompiler);
};