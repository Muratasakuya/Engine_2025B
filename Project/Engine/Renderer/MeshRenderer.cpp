#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(DxCommand* dxCommand, ID3D12Device8* device,
	ShadowMap* shadowMap, DxShaderCompiler* shaderCompiler, SRVManager* srvManager,
	RenderObjectManager* renderObjectManager, CameraManager* cameraManager) {

	commandList_ = nullptr;
	commandList_ = dxCommand->GetCommandList(CommandListType::Graphics);

	dxCommand_ = nullptr;
	dxCommand_ = dxCommand;

	srvManager_ = nullptr;
	srvManager_ = srvManager;

	shadowMap_ = nullptr;
	shadowMap_ = shadowMap;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	renderObjectManager_ = nullptr;
	renderObjectManager_ = renderObjectManager;

	// pipeline作成
	pipeline_ = std::make_unique<ObjectPipelineManager>();
	pipeline_->Create(commandList_, device, shaderCompiler);

	meshShaderPipeline_ = std::make_unique<MeshShaderPipelineState>();
	meshShaderPipeline_->Create(device, shaderCompiler, srvManager);

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

void MeshRenderer::Update() {

	// buffer更新
	viewProjectionBuffer_.TransferData(cameraManager_->GetCamera()->GetViewProjectionMatrix());
	lightViewProjectionBuffer_.TransferData(cameraManager_->GetLightViewCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(cameraManager_->GetCamera()->GetTransform().translation);
	lightBuffer_.TransferData(light_);

#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.TransferData(cameraManager_->GetDebugCamera()->GetViewProjectionMatrix());
	debugSceneCameraPosBuffer_.TransferData(cameraManager_->GetDebugCamera()->GetTransform().translation);
#endif // _DEBUG
}

void MeshRenderer::ZPassRendering() {

}

void MeshRenderer::Rendering(bool debugEnable, ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	const auto& meshes = renderObjectManager_->GetMeshes();
	auto instancingBuffers = renderObjectManager_->GetInstancingData();
	MeshCommandContext commandContext{};

	if (meshes.empty()) {
		return;
	}

	// renderTextureへの描画処理
	// pipeline設定
	commandList->SetGraphicsRootSignature(meshShaderPipeline_->GetRootSignature());
	commandList->SetPipelineState(meshShaderPipeline_->GetPipelineState());

	// 共通のbuffer設定
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
		srvManager_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	// shadowMap
	commandList->SetGraphicsRootDescriptorTable(10,
		shadowMap_->GetGPUHandle());

	// light
	commandList->SetGraphicsRootConstantBufferView(11,
		lightBuffer_.GetResource()->GetGPUVirtualAddress());

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