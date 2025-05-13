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
#include <Engine/Core/Graphics/Skybox/Skybox.h>

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

	// skybox用pipeline作成
	skyboxPipeline_ = std::make_unique<PipelineState>();
	skyboxPipeline_->Create("Skybox.json", device, srvDescriptor, shaderCompiler);
	// skyboxにdeviceを設定
	Skybox::GetInstance()->SetDevice(device);
}

void MeshRenderer::RenderingZPass(GPUObjectSystem* gpuObjectSystem,
	DxCommand* dxCommand) {

	// commandList取得
	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList(CommandListType::Graphics);

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

			// skinnedMeshなら頂点を描画使用できるようにする
			if (mesh->IsSkinned()) {

				dxCommand->TransitionBarriers({ static_cast<SkinnedMesh*>(mesh.get())->GetOutputVertexBuffer(meshIndex).GetResource() },
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}

			// 描画処理
			commandContext.DispatchMesh(commandList,
				instancingBuffers[name].numInstance, meshIndex, mesh.get());
		}
	}
}

void MeshRenderer::Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
	DxCommand* dxCommand) {

	// commandList取得
	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	// 描画情報取得
	const auto& meshes = gpuObjectSystem->GetMeshes();
	auto instancingBuffers = gpuObjectSystem->GetInstancingData();
	MeshCommandContext commandContext{};
	Skybox* skybox = Skybox::GetInstance();

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

	// skyboxがあるときのみ、とりあえず今は
	if (skybox->IsCreated()) {

		// environmentTexture
		commandList->SetGraphicsRootDescriptorTable(11,
			srvDescriptor_->GetGPUHandle(skybox->GetTextureIndex()));
	}

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

#ifdef _DEBUG
			if (debugEnable) {

				// skinnedMeshなら頂点を書き込み状態に戻す
				if (mesh->IsSkinned()) {

					dxCommand->TransitionBarriers({ static_cast<SkinnedMesh*>(mesh.get())->GetOutputVertexBuffer(meshIndex).GetResource() },
						D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				}
			}
#else
			// skinnedMeshなら頂点を書き込み状態に戻す
			if (mesh->IsSkinned()) {

				dxCommand->TransitionBarriers({ static_cast<SkinnedMesh*>(mesh.get())->GetOutputVertexBuffer(meshIndex).GetResource() },
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			}
#endif
		}
	}

	// 作成されていなかったら早期リターン
	if (!skybox->IsCreated()) {
		return;
	}

	// skybox描画
	// pipeline設定
	commandList->SetGraphicsRootSignature(skyboxPipeline_->GetRootSignature());
	commandList->SetPipelineState(skyboxPipeline_->GetGraphicsPipeline());

	// buffer設定
	// vertex
	commandList->IASetVertexBuffers(0, 1, &skybox->GetVertexBuffer().GetVertexBuffer());
	commandList->IASetIndexBuffer(&skybox->GetIndexBuffer().GetIndexBuffer());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// matrix
	commandList->SetGraphicsRootConstantBufferView(0, skybox->GetMatrixBuffer().GetResource()->GetGPUVirtualAddress());
	// viewPro
	gpuObjectSystem->GetSceneBuffer()->SetViewProCommand(debugEnable, commandList, 1);
	// texture
	commandList->SetGraphicsRootDescriptorTable(2,
		srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// material
	commandList->SetGraphicsRootConstantBufferView(3, skybox->GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());

	// 描画
	commandList->DrawIndexedInstanced(skybox->GetIndexCount(), 1, 0, 0, 0);
}