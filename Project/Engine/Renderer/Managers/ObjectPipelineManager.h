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

	void SetInstancingObjectPipeline();

	void SetZPassPipeline();

	ID3D12RootSignature* GetObject3DRootSignature() const { return object3DPipeline_->GetRootSignature(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12GraphicsCommandList* commandList_;

	std::unique_ptr<PipelineState> object3DPipeline_;

	std::unique_ptr<PipelineState> instancingObject3DPipeline_;

	std::unique_ptr<PipelineState> zPassPipeline_;
};