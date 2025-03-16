#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Mesh/MeshCommandContext.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(DxCommand* dxCommand, ID3D12Device* device,
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

	pipeline_ = std::make_unique<ObjectPipelineManager>();
	pipeline_->Create(commandList_, device, shaderCompiler);

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

	viewProjectionBuffer_.TransferData(cameraManager_->GetCamera()->GetViewProjectionMatrix());
	lightViewProjectionBuffer_.TransferData(cameraManager_->GetLightViewCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(cameraManager_->GetCamera()->GetTransform().translation);
	lightBuffer_.TransferData(light_);

#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.TransferData(cameraManager_->GetDebugCamera()->GetViewProjectionMatrix());
	debugSceneCameraPosBuffer_.TransferData(cameraManager_->GetDebugCamera()->GetTransform().translation);
#endif // _DEBUG
}

void MeshRenderer::RenderZPass() {

	// 通常の描画処理
	NormalZPassRendering();
	// instancing描画処理
	InstancingZPassRendering();
}

void MeshRenderer::Render(bool debugEnable) {

	// line描画実行
	LineRenderer::GetInstance()->ExecuteLine(debugEnable);

	// 通常の描画処理
	NormalRendering(debugEnable);
	// instancing描画処理
	InstancingRendering(debugEnable);
}

void MeshRenderer::NormalZPassRendering() {

	// 描画情報取得
	auto object3DBuffers = renderObjectManager_->GetObject3DBuffers();
	MeshCommandContext commandContext{};

	if (object3DBuffers.empty()) {
		return;
	}

	// shadowMapへの描画処理
	pipeline_->SetZPassPipeline();

	commandList_->SetGraphicsRootConstantBufferView(1, lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

	for (auto& object : object3DBuffers) {

		if (object.model.renderingData.instancingEnable) {
			continue;
		}

		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			commandContext.IA(indexCount, meshIndex, object.model, dxCommand_);

			commandList_->SetGraphicsRootConstantBufferView(0, object.matrix.GetResource()->GetGPUVirtualAddress());

			commandContext.Draw(indexCount, object.model, dxCommand_);
		}
	}
}

void MeshRenderer::InstancingZPassRendering() {

	// 描画情報取得
	auto instancingBuffers = renderObjectManager_->GetInstancingData();
	MeshCommandContext commandContext{};

	if (instancingBuffers.empty()) {
		return;
	}

	// shadowMapへの描画処理
	pipeline_->SetInstancingZPassPipeline();

	commandList_->SetGraphicsRootConstantBufferView(1, lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

	for (const auto& buffer : std::views::values(instancingBuffers)) {

		uint32_t meshNum = static_cast<uint32_t>(buffer.model.model->GetMeshNum());
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			commandContext.IA(indexCount, meshIndex, buffer.model, dxCommand_);

			commandList_->SetGraphicsRootDescriptorTable(0, srvManager_->GetGPUHandle(buffer.transformSrvIndex));

			commandContext.InstancingDraw(indexCount, buffer.numInstance, commandList_);
		}
	}
}

void MeshRenderer::NormalRendering(bool debugEnable) {

	// 描画情報取得
	auto object3DBuffers = renderObjectManager_->GetObject3DBuffers();
	MeshCommandContext commandContext{};

	if (object3DBuffers.empty()) {
		return;
	}

	// renderTextureへの描画処理
	pipeline_->SetObjectPipeline();

	// 全object共通のbuffer設定
	commandList_->SetGraphicsRootDescriptorTable(1, shadowMap_->GetGPUHandle());
	if (!debugEnable) {

		commandList_->SetGraphicsRootConstantBufferView(3, viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
		commandList_->SetGraphicsRootConstantBufferView(7, cameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		commandList_->SetGraphicsRootConstantBufferView(3, debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
		commandList_->SetGraphicsRootConstantBufferView(7, debugSceneCameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	}
	commandList_->SetGraphicsRootConstantBufferView(4, lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, lightBuffer_.GetResource()->GetGPUVirtualAddress());

	for (auto& object : object3DBuffers) {

		if (object.model.renderingData.instancingEnable) {
			continue;
		}

		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			commandContext.IA(indexCount, meshIndex, object.model, dxCommand_);

			if (object.model.isAnimation) {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.animationModel->GetTextureGPUHandle(meshIndex));
			} else {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.model->GetTextureGPUHandle(meshIndex));
			}
			commandList_->SetGraphicsRootConstantBufferView(2, object.matrix.GetResource()->GetGPUVirtualAddress());
			commandList_->SetGraphicsRootConstantBufferView(5, object.materials[meshIndex].GetResource()->GetGPUVirtualAddress());

			commandContext.Draw(indexCount, object.model, dxCommand_);
		}
	}
}

void MeshRenderer::InstancingRendering(bool debugEnable) {

	// 描画情報取得
	auto instancingBuffers = renderObjectManager_->GetInstancingData();
	MeshCommandContext commandContext{};

	if (instancingBuffers.empty()) {
		return;
	}

	// renderTextureへの描画処理
	pipeline_->SetInstancingObjectPipeline();

	// 全object共通のbuffer設定
	commandList_->SetGraphicsRootDescriptorTable(3, shadowMap_->GetGPUHandle());
	if (!debugEnable) {

		commandList_->SetGraphicsRootConstantBufferView(4, viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
		commandList_->SetGraphicsRootConstantBufferView(7, cameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		commandList_->SetGraphicsRootConstantBufferView(4, debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
		commandList_->SetGraphicsRootConstantBufferView(7, debugSceneCameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	}
	commandList_->SetGraphicsRootConstantBufferView(5, lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, lightBuffer_.GetResource()->GetGPUVirtualAddress());

	for (const auto& buffer : std::views::values(instancingBuffers)) {

		uint32_t meshNum = static_cast<uint32_t>(buffer.model.model->GetMeshNum());
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			commandContext.IA(indexCount, meshIndex, buffer.model, dxCommand_);

			commandList_->SetGraphicsRootDescriptorTable(0, srvManager_->GetGPUHandle(buffer.transformSrvIndex));
			commandList_->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUHandle(buffer.materialSrvIndices[meshIndex]));
			commandList_->SetGraphicsRootDescriptorTable(2, buffer.model.model->GetTextureGPUHandle(meshIndex));

			commandContext.InstancingDraw(indexCount, buffer.numInstance, commandList_);
		}
	}
}