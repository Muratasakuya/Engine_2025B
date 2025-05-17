#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>

// c++
#include <memory>
#include <ranges>
// front
class SRVDescriptor;
class DxCommand;
class ShadowMap;
class SceneConstBuffer;

//============================================================================
//	MeshRenderer class
//============================================================================
class MeshRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshRenderer() = default;
	~MeshRenderer() = default;

	void Init(ID3D12Device8* device, ShadowMap* shadowMap,
		DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor);

	void RenderingZPass(SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);

	void Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SRVDescriptor* srvDescriptor_;
	ShadowMap* shadowMap_;

	std::unique_ptr<PipelineState> meshShaderPipeline_;
	std::unique_ptr<PipelineState> meshShaderZPassPipeline_;

	std::unique_ptr<PipelineState> skyboxPipeline_;
};