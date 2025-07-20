#include "ParticleRenderer.h"

//============================================================================
//	ParticleRenderer classMethods
//============================================================================

void ParticleRenderer::Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
	DxShaderCompiler* shaderCompiler) {

	// 各pipeline初期化
	InitPipelines(device, srvDescriptor, shaderCompiler);
}

void ParticleRenderer::InitPipelines(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// 平面
	primitivePipelines_[ParticlePrimitiveType::Plane] = std::make_unique<PipelineState>();
	primitivePipelines_[ParticlePrimitiveType::Plane]->Create(
		"EffectPlane.json", device, srvDescriptor, shaderCompiler);
	// リング
	primitivePipelines_[ParticlePrimitiveType::Ring] = std::make_unique<PipelineState>();
	primitivePipelines_[ParticlePrimitiveType::Ring]->Create(
		"EffectRing.json", device, srvDescriptor, shaderCompiler);
	// 円柱
	primitivePipelines_[ParticlePrimitiveType::Cylinder] = std::make_unique<PipelineState>();
	primitivePipelines_[ParticlePrimitiveType::Cylinder]->Create(
		"EffectCylinder.json", device, srvDescriptor, shaderCompiler);
}

void ParticleRenderer::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	// 描画情報を流して描画
	debugEnable;
	sceneBuffer;
	dxCommand;
}