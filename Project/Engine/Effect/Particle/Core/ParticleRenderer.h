#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Effect/Particle/Structures/ParticlePrimitiveStructures.h>
#include <Engine/Effect/Particle/Structures/ParticleStructures.h>

// c++
#include <array>
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

	//--------- variables ----------------------------------------------------

	static const uint32_t kPrimitiveCount = static_cast<uint32_t>(ParticlePrimitiveType::Count);
	static const uint32_t kParticleTypeCount = static_cast<uint32_t>(ParticleType::Count);

	std::array<std::array<std::unique_ptr<PipelineState>, kPrimitiveCount>, kParticleTypeCount> pipelines_;

	//--------- functions ----------------------------------------------------

	void InitPipelines(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
		DxShaderCompiler* shaderCompiler);
};