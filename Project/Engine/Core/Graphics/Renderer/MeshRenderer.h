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

	void Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor);

	void TraceShadowRay(SceneConstBuffer* sceneBuffer,
		class RenderTexture* shadowRayTexture, DxCommand* dxCommand);

	void Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer,
		RenderTexture* shadowRayTexture, DxCommand* dxCommand);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SRVDescriptor* srvDescriptor_;

	// pipelines
	// shadowRayQuery
	std::unique_ptr<RaytracingPipeline> shadowRayPipeline_;
	// main
	std::unique_ptr<PipelineState> meshShaderPipeline_;
	// skybox
	std::unique_ptr<PipelineState> skyboxPipeline_;

	// raytracing
	std::unique_ptr<RaytracingScene> rayScene_;
};