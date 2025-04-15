#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>

// c++
#include <memory>
// front
class SRVDescriptor;
class GPUObjectSystem;

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
	
	std::unique_ptr<PipelineState> meshShaderPipeline_;
};