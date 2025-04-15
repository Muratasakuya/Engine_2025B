#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Engine/Scene/Camera/CameraManager.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(ID3D12Device8* device, ShadowMap* shadowMap,
	DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor) {

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	shadowMap_ = nullptr;
	shadowMap_ = shadowMap;

	// pipeline作成
	meshShaderPipeline_ = std::make_unique<PipelineState>();
	meshShaderPipeline_->Create("MeshStandard.json", device, srvDescriptor, shaderCompiler);

	meshShaderZPassPipeline_ = std::make_unique<PipelineState>();
	meshShaderZPassPipeline_->Create("MeshDepth.json", device, srvDescriptor, shaderCompiler);
}

void MeshRenderer::RenderingZPass(GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	const auto& meshes = gpuObjectSystem->GetMeshes();
	auto instancingBuffers = gpuObjectSystem->GetInstancingData();
	MeshCommandContext commandContext{};

	if (meshes.empty()) {
		return;
	}

	// renderTextureへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(meshShaderZPassPipeline_->GetRootSignature());
	commandList->SetPipelineState(meshShaderZPassPipeline_->GetGraphicsPipeline());

	// lightViewProjection
	gpuObjectSystem->GetSceneBuffer()->SetZPassCommands(commandList);

	for (const auto& [name, mesh] : meshes) {

		// meshごとのmatrix設定
		commandList->SetGraphicsRootShaderResourceView(4,
			instancingBuffers[name].matrix.GetResource()->GetGPUVirtualAddress());

		for (uint32_t meshIndex = 0; meshIndex < mesh->GetMeshCount(); ++meshIndex) {

			// 描画処理
			commandContext.DispatchMesh(commandList,
				instancingBuffers[name].numInstance, meshIndex, mesh.get());
		}
	}
}

void MeshRenderer::Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	const auto& meshes = gpuObjectSystem->GetMeshes();
	auto instancingBuffers = gpuObjectSystem->GetInstancingData();
	MeshCommandContext commandContext{};

	if (meshes.empty()) {
		return;
	}

	// renderTextureへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(meshShaderPipeline_->GetRootSignature());
	commandList->SetPipelineState(meshShaderPipeline_->GetGraphicsPipeline());

	// 共通のbuffer設定
	gpuObjectSystem->GetSceneBuffer()->SetMainPassCommands(debugEnable, commandList);
	// allTexture
	commandList->SetGraphicsRootDescriptorTable(9,
		srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// shadowMap
	commandList->SetGraphicsRootDescriptorTable(10,
		shadowMap_->GetGPUHandle());

	for (const auto& [name, mesh] : meshes) {

		// meshごとのmatrix設定
		commandList->SetGraphicsRootShaderResourceView(4,
			instancingBuffers[name].matrix.GetResource()->GetGPUVirtualAddress());

		for (uint32_t meshIndex = 0; meshIndex < mesh->GetMeshCount(); ++meshIndex) {

			// meshごとのmaterial設定
			commandList->SetGraphicsRootShaderResourceView(8,
				instancingBuffers[name].materials[meshIndex].GetResource()->GetGPUVirtualAddress());

			// 描画処理
			commandContext.DispatchMesh(commandList,
				instancingBuffers[name].numInstance, meshIndex, mesh.get());
		}
	}
}