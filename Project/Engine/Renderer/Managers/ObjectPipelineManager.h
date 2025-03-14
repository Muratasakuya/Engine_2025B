#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>

// c++
#include <memory>
// front
class DxCommand;

//============================================================================
//	ObjectPipelineManager class
//============================================================================
class ObjectPipelineManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ObjectPipelineManager() = default;
	~ObjectPipelineManager() = default;

	void Create(ID3D12GraphicsCommandList* commandList, ID3D12Device* device,
		DxShaderCompiler* shaderCompiler);

	//--------- accessor -----------------------------------------------------

	void SetObjectPipeline();

	void SetZPassPipeline();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12GraphicsCommandList* commandList_;

	std::unique_ptr<PipelineState> object3DPipeline_;

	std::unique_ptr<PipelineState> zPassPipeline_;
};