#include "SpriteRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	SpriteRenderer classMethods
//============================================================================

void SpriteRenderer::Init(ID3D12Device8* device, SRVManager* srvManager,
	DxShaderCompiler* shaderCompiler) {

	// pipeline作成
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->Create(
		"IrrelevantPostProcessObject2D.json", device, srvManager, shaderCompiler);

	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->Create(
		"ApplyPostProcessObject2D.json", device, srvManager, shaderCompiler);

	// buffer作成
	viewProjectionBuffer_.CreateConstBuffer(device);
}

void SpriteRenderer::Update(CameraManager* cameraManager) {

	// buffer更新
	viewProjectionBuffer_.TransferData(cameraManager->GetCamera2D()->GetViewProjectionMatrix());
}

void SpriteRenderer::RenderIrrelevant(GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	auto object2DBuffers = gpuObjectSystem->GetObject2DBuffers();

	if (object2DBuffers.empty()) {
		return;
	}

	// renderModeに応じたpipeline設定
	commandList->SetGraphicsRootSignature(pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->GetGraphicsPipeline());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 全object共通のbuffer設定
	commandList->SetGraphicsRootConstantBufferView(2,
		viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	// indexBuffer設定
	commandList->IASetIndexBuffer(&object2DBuffers.front().
		sprite.sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : object2DBuffers) {
		if (buffer.sprite.sprite->GetPostProcessEnable()) {
			continue;
		}

		commandList->IASetVertexBuffers(0, 1, &buffer.sprite.sprite->GetVertexBuffer().GetVertexBuffer());

		commandList->SetGraphicsRootDescriptorTable(0, buffer.sprite.sprite->GetTextureGPUHandle());
		commandList->SetGraphicsRootConstantBufferView(1, buffer.matrix.GetResource()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(3, buffer.material.GetResource()->GetGPUVirtualAddress());

		commandList->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
	}
}

void SpriteRenderer::RenderApply(SpriteLayer layer, GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	auto object2DBuffers = gpuObjectSystem->GetObject2DBuffers();

	if (object2DBuffers.empty()) {
		return;
	}

	// renderModeに応じたpipeline設定
	commandList->SetGraphicsRootSignature(pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->GetGraphicsPipeline());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 全object共通のbuffer設定
	commandList->SetGraphicsRootConstantBufferView(2,
		viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	// indexBuffer設定
	commandList->IASetIndexBuffer(&object2DBuffers.front().
		sprite.sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : object2DBuffers) {
		if (buffer.sprite.sprite->GetLayer() == layer) {
			if (!buffer.sprite.sprite->GetPostProcessEnable()) {
				continue;
			}

			commandList->IASetVertexBuffers(0, 1, &buffer.sprite.sprite->GetVertexBuffer().GetVertexBuffer());

			commandList->SetGraphicsRootDescriptorTable(0, buffer.sprite.sprite->GetTextureGPUHandle());
			commandList->SetGraphicsRootConstantBufferView(1, buffer.matrix.GetResource()->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(3, buffer.material.GetResource()->GetGPUVirtualAddress());

			commandList->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
		}
	}
}