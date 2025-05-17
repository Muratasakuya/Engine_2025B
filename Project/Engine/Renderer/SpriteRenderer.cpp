#include "SpriteRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Camera/CameraManager.h>

//============================================================================
//	SpriteRenderer classMethods
//============================================================================

void SpriteRenderer::Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
	DxShaderCompiler* shaderCompiler) {

	// pipeline作成
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->Create(
		"IrrelevantPostProcessObject2D.json", device, srvDescriptor, shaderCompiler);

	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->Create(
		"ApplyPostProcessObject2D.json", device, srvDescriptor, shaderCompiler);

	// buffer作成
	viewProjectionBuffer_.CreateConstBuffer(device);
}

void SpriteRenderer::Update(CameraManager* cameraManager) {

	// buffer更新
	viewProjectionBuffer_.TransferData(cameraManager->GetCamera2D()->GetViewProjectionMatrix());
}