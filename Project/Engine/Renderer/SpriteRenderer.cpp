#include "SpriteRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	SpriteRenderer classMethods
//============================================================================

void SpriteRenderer::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	DxShaderCompiler* shaderCompiler, GPUObjectSystem* gpuObjectSystem,
	CameraManager* cameraManager) {

	commandList_ = nullptr;
	commandList_ = commandList;

	gpuObjectSystem_ = nullptr;
	gpuObjectSystem_ = gpuObjectSystem;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	// pipeline作成
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->Create(
		"IrrelevantPostProcessObject2D.json", device, shaderCompiler);

	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)] =
		std::make_unique<PipelineState>();
	pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->Create(
		"ApplyPostProcessObject2D.json", device, shaderCompiler);

	// buffer作成
	viewProjectionBuffer_.CreateConstBuffer(device);
}

void SpriteRenderer::Update() {

	// buffer更新
	viewProjectionBuffer_.TransferData(cameraManager_->GetCamera2D()->GetViewProjectionMatrix());
}

void SpriteRenderer::RenderIrrelevant() {

	// 描画情報取得
	auto object2DBuffers = gpuObjectSystem_->GetObject2DBuffers();

	if (object2DBuffers.empty()) {
		return;
	}

	// renderModeに応じたpipeline設定
	commandList_->SetGraphicsRootSignature(pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->GetRootSignature());
	commandList_->SetPipelineState(pipelines_[static_cast<uint32_t>(RenderMode::IrrelevantPostProcess)]->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 全object共通のbuffer設定
	commandList_->SetGraphicsRootConstantBufferView(2,
		viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	// indexBuffer設定
	commandList_->IASetIndexBuffer(&object2DBuffers.front().
		sprite.sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : object2DBuffers) {
		if (buffer.sprite.sprite->GetPostProcessEnable()) {
			continue;
		}

		commandList_->IASetVertexBuffers(0, 1, &buffer.sprite.sprite->GetVertexBuffer().GetVertexBuffer());

		commandList_->SetGraphicsRootDescriptorTable(0, buffer.sprite.sprite->GetTextureGPUHandle());
		commandList_->SetGraphicsRootConstantBufferView(1, buffer.matrix.GetResource()->GetGPUVirtualAddress());
		commandList_->SetGraphicsRootConstantBufferView(3, buffer.material.GetResource()->GetGPUVirtualAddress());

		commandList_->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
	}
}

void SpriteRenderer::RenderApply(SpriteLayer layer) {

	// 描画情報取得
	auto object2DBuffers = gpuObjectSystem_->GetObject2DBuffers();

	if (object2DBuffers.empty()) {
		return;
	}

	// renderModeに応じたpipeline設定
	commandList_->SetGraphicsRootSignature(pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->GetRootSignature());
	commandList_->SetPipelineState(pipelines_[static_cast<uint32_t>(RenderMode::ApplyPostProcess)]->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 全object共通のbuffer設定
	commandList_->SetGraphicsRootConstantBufferView(2,
		viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	// indexBuffer設定
	commandList_->IASetIndexBuffer(&object2DBuffers.front().
		sprite.sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : object2DBuffers) {
		if (buffer.sprite.sprite->GetLayer() == layer) {
			if (!buffer.sprite.sprite->GetPostProcessEnable()) {
				continue;
			}

			commandList_->IASetVertexBuffers(0, 1, &buffer.sprite.sprite->GetVertexBuffer().GetVertexBuffer());

			commandList_->SetGraphicsRootDescriptorTable(0, buffer.sprite.sprite->GetTextureGPUHandle());
			commandList_->SetGraphicsRootConstantBufferView(1, buffer.matrix.GetResource()->GetGPUVirtualAddress());
			commandList_->SetGraphicsRootConstantBufferView(3, buffer.material.GetResource()->GetGPUVirtualAddress());

			commandList_->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
		}
	}
}