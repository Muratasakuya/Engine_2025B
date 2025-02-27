#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComPtr.h>
#include <Engine/Core/Lib/DxStructures.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>

// directX
#include <d3d12.h>
// c++
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// json
#include <Externals/nlohmann/json.hpp>
// using
using Json = nlohmann::json;

//============================================================================
//	PipelineState class
//============================================================================
class PipelineState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PipelineState() = default;
	virtual ~PipelineState() = default;

	void Create(const std::string& fileName, ID3D12Device* device,
		DxShaderCompiler* shaderCompiler);

	//--------- accessor -----------------------------------------------------

	ID3D12PipelineState* GetGraphicsPipeline(BlendMode blendMode = BlendMode::kBlendModeNormal) const;
	ID3D12PipelineState* GetComputePipeline() const { return computePipelineState_.Get(); };

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<ComPtr<ID3D12PipelineState>> graphicsPipelinepipelineStates_;

	ComPtr<ID3D12PipelineState> computePipelineState_;

	ComPtr<ID3D12RootSignature> rootSignature_;

	//--------- functions ----------------------------------------------------

	Json LoadFile(const std::string& fileName);
};