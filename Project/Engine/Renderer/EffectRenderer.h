#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>

// c++
#include <memory>
// front
class SRVDescriptor;

//============================================================================
//	EffectRenderer class
//============================================================================
class EffectRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EffectRenderer() = default;
	~EffectRenderer() = default;

	void Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler,
		SRVDescriptor* srvDescriptor);

	void Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
		ID3D12GraphicsCommandList6* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SRVDescriptor* srvDescriptor_;

	std::unordered_map<BlendMode, std::vector<EffectForGPU>> buffers_;
	size_t bufferCount_;
	std::vector<BlendMode> prevBlendModes_;

	std::unique_ptr<PipelineState> meshShaderPipeline_;

	//--------- functionsc ----------------------------------------------------

	void UpdateBuffers(const std::vector<EffectForGPU>& effectBuffers);
	bool UpdateBlendMode(const std::vector<EffectForGPU>& effectBuffers);
};