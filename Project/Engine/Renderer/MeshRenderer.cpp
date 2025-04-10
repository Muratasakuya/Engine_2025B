#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Game/Camera/Manager/CameraManager.h>

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

	// buffer作成
	viewProjectionBuffer_.CreateConstBuffer(device);
	lightViewProjectionBuffer_.CreateConstBuffer(device);
	cameraPosBuffer_.CreateConstBuffer(device);
	light_.Init();
	lightBuffer_.CreateConstBuffer(device);

#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.CreateConstBuffer(device);
	debugSceneCameraPosBuffer_.CreateConstBuffer(device);
#endif // _DEBUG
}

void MeshRenderer::Update(CameraManager* cameraManager) {

	// buffer更新
	viewProjectionBuffer_.TransferData(cameraManager->GetCamera()->GetViewProjectionMatrix());
	lightViewProjectionBuffer_.TransferData(cameraManager->GetLightViewCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(cameraManager->GetCamera()->GetTransform().translation);
	lightBuffer_.TransferData(light_);

#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.TransferData(cameraManager->GetDebugCamera()->GetViewProjectionMatrix());
	debugSceneCameraPosBuffer_.TransferData(cameraManager->GetDebugCamera()->GetTransform().translation);
#endif // _DEBUG
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
	commandList->SetGraphicsRootConstantBufferView(6,
		lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

	for (const auto& [name, mesh] : meshes) {

		// meshごとのmatrix設定
		commandList->SetGraphicsRootShaderResourceView(4,
			instancingBuffers[name].matrix.GetResource()->GetGPUVirtualAddress());

		for (uint32_t meshIndex = 0; meshIndex < mesh->GetMeshCount(); ++meshIndex) {

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
	SetCommonBuffer(debugEnable, commandList);

	for (const auto& [name, mesh] : meshes) {

		// meshごとのmatrix設定
		commandList->SetGraphicsRootShaderResourceView(4,
			instancingBuffers[name].matrix.GetResource()->GetGPUVirtualAddress());

		for (uint32_t meshIndex = 0; meshIndex < mesh->GetMeshCount(); ++meshIndex) {

			// meshごとのmaterial設定
			commandList->SetGraphicsRootShaderResourceView(8,
				instancingBuffers[name].materials[meshIndex].GetResource()->GetGPUVirtualAddress());

			commandContext.DispatchMesh(commandList,
				instancingBuffers[name].numInstance, meshIndex, mesh.get());
		}
	}
}

void MeshRenderer::SetCommonBuffer(bool debugEnable, ID3D12GraphicsCommandList6* commandList) {

	if (!debugEnable) {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(6,
			viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

		// cameraPos
		commandList->SetGraphicsRootConstantBufferView(12,
			cameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(6,
			debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

		// cameraPos
		commandList->SetGraphicsRootConstantBufferView(12,
			debugSceneCameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	}
	// lightViewProjection
	commandList->SetGraphicsRootConstantBufferView(7,
		lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

	// allTexture
	commandList->SetGraphicsRootDescriptorTable(9,
		srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// shadowMap
	commandList->SetGraphicsRootDescriptorTable(10,
		shadowMap_->GetGPUHandle());

	// light
	commandList->SetGraphicsRootConstantBufferView(11,
		lightBuffer_.GetResource()->GetGPUVirtualAddress());
}