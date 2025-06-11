#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingPipeline.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingScene.h>

// c++
#include <memory>
#include <ranges>
// front
class SRVDescriptor;
class DxCommand;
class RenderTexture;
class DepthTexture;
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

	void Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor);

	void Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SRVDescriptor* srvDescriptor_;

	// main
	std::unique_ptr<PipelineState> meshShaderPipeline_;
	// skybox
	std::unique_ptr<PipelineState> skyboxPipeline_;

	// raytracing
	std::unique_ptr<RaytracingScene> rayScene_;
};