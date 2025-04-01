#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessType.h>

// c++
#include <memory>
#include <array>

//============================================================================
//	PostProcessPipelineManager class
//============================================================================
class PostProcessPipelineManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PostProcessPipelineManager() = default;
	~PostProcessPipelineManager() = default;

	void Init(ID3D12Device* device, class DxShaderCompiler* shaderCompiler);

	//--------- accessor -----------------------------------------------------

	void SetPipeline(ID3D12GraphicsCommandList* commandList, PostProcessType type);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::array<std::unique_ptr<PipelineState>, kPostProcessCount> pipelines_;
};