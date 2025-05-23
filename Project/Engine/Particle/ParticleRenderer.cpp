#include "ParticleRenderer.h"

//============================================================================
//	ParticleRenderer classMethods
//============================================================================

void ParticleRenderer::Init(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// pipeline作成
	pipeline_ = std::make_unique<PipelineState>();
	pipeline_->Create("", device, srvDescriptor, shaderCompiler);
}