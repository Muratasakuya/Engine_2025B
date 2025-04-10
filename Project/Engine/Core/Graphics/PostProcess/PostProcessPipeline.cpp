#include "PostProcessPipeline.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	PostProcessPipeline classMethods
//============================================================================

void PostProcessPipeline::Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
	DxShaderCompiler* shaderCompiler) {

	// shaderDataFileName
	std::vector<std::string> fileNames = {
		"HorizontalBlur.json",
		"VerticalBlur.json",
		"RadialBlur.json",
		"GaussianFilter.json",
		"BoxFilter.json",
		"Dissolve.json",
		"Random.json",
		"Vignette.json",
		"Grayscale.json",
		"SepiaTone.json",
		"BloomLuminanceExtract.json",
		"BloomCombine.json",
		"LuminanceBasedOutline.json",
		"DepthBasedOutline.json",
	};

	for (const uint32_t& type : Algorithm::GetEnumArray(PostProcessType::Count)) {

		pipelines_[type] = std::make_unique<PipelineState>();
		pipelines_[type]->Create(fileNames[type], device, srvDescriptor, shaderCompiler);
	}
}

void PostProcessPipeline::SetPipeline(ID3D12GraphicsCommandList* commandList, PostProcessType type) {

	commandList->SetComputeRootSignature(pipelines_[static_cast<uint32_t>(type)]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[static_cast<uint32_t>(type)]->GetComputePipeline());
}