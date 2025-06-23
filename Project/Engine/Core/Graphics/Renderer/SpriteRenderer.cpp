#include "SpriteRenderer.h"

//============================================================================
//	include
//============================================================================
// Graphics
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>

// ECS
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/System/Systems/SpriteBufferSystem.h>

//============================================================================
//	SpriteRenderer classMethods
//============================================================================

void SpriteRenderer::Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
	DxShaderCompiler* shaderCompiler) {

	// pipeline作成
	pipelines_[RenderMode::IrrelevantPostProcess] = std::make_unique<PipelineState>();
	pipelines_[RenderMode::IrrelevantPostProcess]->Create(
		"IrrelevantPostProcessObject2D.json", device, srvDescriptor, shaderCompiler);

	pipelines_[RenderMode::ApplyPostProcess] = std::make_unique<PipelineState>();
	pipelines_[RenderMode::ApplyPostProcess]->Create(
		"ApplyPostProcessObject2D.json", device, srvDescriptor, shaderCompiler);
}

void SpriteRenderer::ApplyPostProcessRendering(SpriteLayer layer,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	// 描画情報取得
	const auto& ecsSystem = ECSManager::GetInstance()->GetSystem<SpriteBufferSystem>();
	const auto& spriteData = ecsSystem->GetSpriteData(layer);

	if (spriteData.empty()) {
		return;
	}

	// commandList取得
	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// frameBufferへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(pipelines_[RenderMode::ApplyPostProcess]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[RenderMode::ApplyPostProcess]->GetGraphicsPipeline());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// lightViewProjection
	sceneBuffer->SetOrthoProCommand(commandList, 3);
	// index
	commandList->IASetIndexBuffer(&spriteData.front().sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : spriteData) {

		// postProcessをかけないのかチェック
		if (!buffer.sprite->GetPostProcessEnable()) {
			// かけない場合は処理しない
			continue;
		}

		// vertex
		commandList->IASetVertexBuffers(0, 1, &buffer.sprite->GetVertexBuffer().GetVertexBuffer());

		// texture
		commandList->SetGraphicsRootDescriptorTable(0, buffer.sprite->GetTextureGPUHandle());
		// alphaTexture
		if (buffer.sprite->UseAlphaTexture()) {

			commandList->SetGraphicsRootDescriptorTable(1, buffer.sprite->GetAlphaTextureGPUHandle());
		}
		// matrix
		commandList->SetGraphicsRootConstantBufferView(2, buffer.transform->GetBuffer().GetResource()->GetGPUVirtualAddress());
		// material
		commandList->SetGraphicsRootConstantBufferView(4, buffer.material->GetBuffer().GetResource()->GetGPUVirtualAddress());

		// 描画処理
		commandList->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
	}
}

void SpriteRenderer::IrrelevantRendering(SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	// 描画情報取得
	const auto& ecsSystem = ECSManager::GetInstance()->GetSystem<SpriteBufferSystem>();
	const auto& spriteData = ecsSystem->GetSpriteData(SpriteLayer::PostModel);

	if (spriteData.empty()) {
		return;
	}

	// commandList取得
	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// frameBufferへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(pipelines_[RenderMode::IrrelevantPostProcess]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[RenderMode::IrrelevantPostProcess]->GetGraphicsPipeline());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// lightViewProjection
	sceneBuffer->SetOrthoProCommand(commandList, 3);
	// index
	commandList->IASetIndexBuffer(&spriteData.front().sprite->GetIndexBuffer().GetIndexBuffer());

	for (const auto& buffer : spriteData) {

		// postProcessをかけないのかチェック
		if (buffer.sprite->GetPostProcessEnable()) {
			// かける場合は処理しない
			continue;
		}

		// vertex
		commandList->IASetVertexBuffers(0, 1, &buffer.sprite->GetVertexBuffer().GetVertexBuffer());

		// texture
		commandList->SetGraphicsRootDescriptorTable(0, buffer.sprite->GetTextureGPUHandle());
		// alphaTexture
		if (buffer.sprite->UseAlphaTexture()) {

			commandList->SetGraphicsRootDescriptorTable(1, buffer.sprite->GetAlphaTextureGPUHandle());
		}
		// matrix
		commandList->SetGraphicsRootConstantBufferView(2, buffer.transform->GetBuffer().GetResource()->GetGPUVirtualAddress());
		// material
		commandList->SetGraphicsRootConstantBufferView(4, buffer.material->GetBuffer().GetResource()->GetGPUVirtualAddress());

		// 描画処理
		commandList->DrawIndexedInstanced(SpriteComponent::GetIndexNum(), 1, 0, 0, 0);
	}
}