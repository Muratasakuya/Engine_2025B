#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
// Graphics
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/PostProcess/RenderTexture.h>
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Config.h>

// ECS
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/System/Systems/InstancedMeshSystem.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor) {

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	// pipeline作成
	shadowRayPipeline_ = std::make_unique<RaytracingPipeline>();
	shadowRayPipeline_->Init(device, shaderCompiler);

	meshShaderPipeline_ = std::make_unique<PipelineState>();
	meshShaderPipeline_->Create("MeshStandard.json", device, srvDescriptor, shaderCompiler);

	// skybox用pipeline作成
	skyboxPipeline_ = std::make_unique<PipelineState>();
	skyboxPipeline_->Create("Skybox.json", device, srvDescriptor, shaderCompiler);
	// skyboxにdeviceを設定
	Skybox::GetInstance()->SetDevice(device);

	// rayScene初期化
	rayScene_ = std::make_unique<RaytracingScene>();
	rayScene_->Init(device);
}

void MeshRenderer::TraceShadowRay(SceneConstBuffer* sceneBuffer,
	RenderTexture* shadowRayTexture, DxCommand* dxCommand) {

	// commandList取得
	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();

	// 描画情報取得
	const auto& ecsSystem = ECSManager::GetInstance()->GetSystem<InstancedMeshSystem>();
	const auto& meshes = ecsSystem->GetMeshes();
	auto instancingBuffers = ecsSystem->GetInstancingData();

	if (meshes.empty()) {
		return;
	}

	// TLAS更新処理
	std::vector<IMesh*> meshPtrs;
	meshPtrs.reserve(meshes.size());
	for (auto& [_, mesh] : meshes) {

		meshPtrs.emplace_back(mesh.get());
	}

	// BLAS更新
	rayScene_->BuildBLASes(commandList, meshPtrs);
	std::vector<RayTracingInstance> rtInstances = ecsSystem->CollectRTInstances(rayScene_.get());
	// TLAS更新
	rayScene_->BuildTLAS(commandList, rtInstances);

	// pipeline設定
	commandList->SetComputeRootSignature(shadowRayPipeline_->GetRootSignature());
	commandList->SetPipelineState1(shadowRayPipeline_->GetPipelineState());

	// TLAS
	commandList->SetComputeRootShaderResourceView(0, rayScene_->GetTLASResource()->GetGPUVirtualAddress());
	// UAV
	commandList->SetComputeRootDescriptorTable(1, shadowRayTexture->GetUAVGPUHandle());
	// scene情報
	sceneBuffer->SetRaySceneCommand(commandList, 2);

	// rayDesc設定
	D3D12_DISPATCH_RAYS_DESC desc = {};
	ID3D12Resource* shaderTable = shadowRayPipeline_->GetShaderTable();

	const UINT64 base = shaderTable->GetGPUVirtualAddress();

	// RayGeneration
	desc.RayGenerationShaderRecord.StartAddress = base + RaytracingPipeline::kRayGenOffset;
	desc.RayGenerationShaderRecord.SizeInBytes = RaytracingPipeline::kHandleSize;
	// Miss
	desc.MissShaderTable.StartAddress = base + RaytracingPipeline::kMissOffset;
	desc.MissShaderTable.StrideInBytes = RaytracingPipeline::kRecordStride;
	desc.MissShaderTable.SizeInBytes = RaytracingPipeline::kRecordStride;
	// HitGroup 
	desc.HitGroupTable.StartAddress = base + RaytracingPipeline::kHitGroupOffset;
	desc.HitGroupTable.StrideInBytes = RaytracingPipeline::kRecordStride;
	desc.HitGroupTable.SizeInBytes = RaytracingPipeline::kRecordStride;

	desc.Width = Config::kWindowWidth;
	desc.Height = Config::kWindowHeight;
	desc.Depth = 1;

	// 実行
	commandList->DispatchRays(&desc);
}

void MeshRenderer::Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer,
	RenderTexture* shadowRayTexture, DxCommand* dxCommand) {

	// commandList取得
	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();

	// 描画情報取得
	const auto& ecsSystem = ECSManager::GetInstance()->GetSystem<InstancedMeshSystem>();
	MeshCommandContext commandContext{};
	Skybox* skybox = Skybox::GetInstance();

	const auto& meshes = ecsSystem->GetMeshes();
	auto instancingBuffers = ecsSystem->GetInstancingData();

	if (meshes.empty()) {
		return;
	}

	// renderTextureへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(meshShaderPipeline_->GetRootSignature());
	commandList->SetPipelineState(meshShaderPipeline_->GetGraphicsPipeline());

	// 共通のbuffer設定
	sceneBuffer->SetMainPassCommands(debugEnable, commandList);
	// allTexture
	commandList->SetGraphicsRootDescriptorTable(10, srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// shadowTexture
	commandList->SetGraphicsRootDescriptorTable(11, shadowRayTexture->GetSRVGPUHandle());

	// skyboxがあるときのみ、とりあえず今は
	if (skybox->IsCreated()) {

		// environmentTexture
		commandList->SetGraphicsRootDescriptorTable(12,
			srvDescriptor_->GetGPUHandle(skybox->GetTextureIndex()));
	}

	for (const auto& [name, mesh] : meshes) {

		// meshごとのmatrix設定
		commandList->SetGraphicsRootShaderResourceView(4,
			instancingBuffers[name].matrixBuffer.GetResource()->GetGPUVirtualAddress());

		for (uint32_t meshIndex = 0; meshIndex < mesh->GetMeshCount(); ++meshIndex) {

			// meshごとのmaterial、lighting設定
			commandList->SetGraphicsRootShaderResourceView(8,
				instancingBuffers[name].materialsBuffer[meshIndex].GetResource()->GetGPUVirtualAddress());
			commandList->SetGraphicsRootShaderResourceView(9,
				instancingBuffers[name].lightingBuffer[meshIndex].GetResource()->GetGPUVirtualAddress());

			// skinnedMeshなら頂点を描画使用できるようにする
			if (mesh->IsSkinned()) {

				dxCommand->TransitionBarriers({ static_cast<SkinnedMesh*>(mesh.get())->GetOutputVertexBuffer(meshIndex).GetResource() },
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}

			// 描画処理
			commandContext.DispatchMesh(commandList,
				instancingBuffers[name].numInstance, meshIndex, mesh.get());

#if defined(_DEBUG) || defined(_DEVELOPBUILD)
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
	sceneBuffer->SetViewProCommand(debugEnable, commandList, 1);
	// texture
	commandList->SetGraphicsRootDescriptorTable(2,
		srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// material
	commandList->SetGraphicsRootConstantBufferView(3, skybox->GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());

	// 描画
	commandList->DrawIndexedInstanced(skybox->GetIndexCount(), 1, 0, 0, 0);
}