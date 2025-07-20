#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/GPUEffect/Particle/Structures/ParticlePrimitiveStructures.h>

// front
class SRVDescriptor;
class DxShaderCompiler;
class DxCommand;
class SceneConstBuffer;

//============================================================================
//	ParticleRenderer class
//============================================================================
class ParticleRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleRenderer() = default;
	~ParticleRenderer() = default;

	void Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
		DxShaderCompiler* shaderCompiler);

	void Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	using PrimitivePipeline = std::unordered_map<ParticlePrimitiveType, std::unique_ptr<PipelineState>>;

	//--------- variables ----------------------------------------------------

	PrimitivePipeline primitivePipelines_;

	//--------- functions ----------------------------------------------------

	void InitPipelines(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
		DxShaderCompiler* shaderCompiler);
};