#include "EffectRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>

//============================================================================
//	EffectRenderer classMethods
//============================================================================

void EffectRenderer::Init(ID3D12Device8* device,
	DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor) {

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	// pipeline作成
	meshShaderPipeline_ = std::make_unique<PipelineState>();
	meshShaderPipeline_->Create("EffectMesh.json", device, srvDescriptor, shaderCompiler);
}

void EffectRenderer::Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	const auto& effectBuffers = gpuObjectSystem->GetEffectBuffers();
	MeshCommandContext commandContext{};

	if (effectBuffers.empty()) {
		return;
	}

	// renderTextureへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(meshShaderPipeline_->GetRootSignature());
	commandList->SetPipelineState(meshShaderPipeline_->GetGraphicsPipeline());

	// 共通のbuffer設定
	gpuObjectSystem->GetSceneBuffer()->SetViewProCommand(debugEnable, commandList);
	// allTexture
	commandList->SetGraphicsRootDescriptorTable(6,
		srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	for (const auto& buffer : effectBuffers) {

		// matrix設定
		commandList->SetGraphicsRootConstantBufferView(4,
			buffer.matrix.GetResource()->GetGPUVirtualAddress());

		// material設定
		commandList->SetGraphicsRootConstantBufferView(7,
			buffer.material.GetResource()->GetGPUVirtualAddress());

		commandContext.DispatchMesh(commandList, buffer.primitiveMesh);
	}
}