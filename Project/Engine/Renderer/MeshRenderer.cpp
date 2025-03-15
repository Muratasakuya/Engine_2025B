#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/ShadowMap.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(DxCommand* dxCommand, ID3D12Device* device,
	ShadowMap* shadowMap, DxShaderCompiler* shaderCompiler,
	RenderObjectManager* renderObjectManager, CameraManager* cameraManager) {

	commandList_ = nullptr;
	commandList_ = dxCommand->GetCommandList(CommandListType::Graphics);

	dxCommand_ = nullptr;
	dxCommand_ = dxCommand;

	shadowMap_ = nullptr;
	shadowMap_ = shadowMap;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	renderObjectManager_ = nullptr;
	renderObjectManager_ = renderObjectManager;

	pipeline_ = std::make_unique<ObjectPipelineManager>();
	pipeline_->Create(commandList_, device, shaderCompiler);

	viewProjectionBuffer_.CreateConstBuffer(device, 3);
	lightViewProjectionBuffer_.CreateConstBuffer(device, 1);
	cameraPosBuffer_.CreateConstBuffer(device, 7);
	light_.Init();
	lightBuffer_.CreateConstBuffer(device, 6);

#ifdef _DEBUG
	debugSceneViewProjectionBuffer_.CreateConstBuffer(device, 3);
	debugSceneCameraPosBuffer_.CreateConstBuffer(device, 7);
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

	// 描画情報取得
	auto object3DBuffers = renderObjectManager_->GetObject3DBuffers();

	if (object3DBuffers.empty()) {
		return;
	}

	// shadowMapへの描画処理
	pipeline_->SetZPassPipeline();

	lightViewProjectionBuffer_.SetCommand(commandList_, 1);

	for (auto& object : object3DBuffers) {

		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			MeshCommand::IA(indexCount, meshIndex, object.model, dxCommand_);

			object.matrix.SetCommand(commandList_, 0);

			MeshCommand::Draw(indexCount, object.model, dxCommand_);
		}
	}
}

void MeshRenderer::Render(bool debugEnable) {

	// line描画実行
	LineRenderer::GetInstance()->ExecuteLine(debugEnable);

	// 描画情報取得
	auto object3DBuffers = renderObjectManager_->GetObject3DBuffers();

	if (object3DBuffers.empty()) {
		return;
	}

	// renderTextureへの描画処理
	pipeline_->SetObjectPipeline();

	commandList_->SetGraphicsRootDescriptorTable(1, shadowMap_->GetGPUHandle());
	if (!debugEnable) {

		viewProjectionBuffer_.SetCommand(commandList_);
		cameraPosBuffer_.SetCommand(commandList_);
	} else {

		debugSceneViewProjectionBuffer_.SetCommand(commandList_);
		debugSceneCameraPosBuffer_.SetCommand(commandList_);
	}
	lightViewProjectionBuffer_.SetCommand(commandList_, 4);
	lightBuffer_.SetCommand(commandList_);

	for (auto& object : object3DBuffers) {

		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			MeshCommand::IA(indexCount, meshIndex, object.model, dxCommand_);

			if (object.model.isAnimation) {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.animationModel->GetTextureGPUHandle(meshIndex));
			} else {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.model->GetTextureGPUHandle(meshIndex));
			}
			object.matrix.SetCommand(commandList_, 2);
			object.materials[meshIndex].SetCommand(commandList_);

			MeshCommand::Draw(indexCount, object.model, dxCommand_);
		}
	}
}

//============================================================================
//	MeshCommand namespaceMethods
//============================================================================

void MeshCommand::IA(UINT& indexCount, uint32_t meshIndex, const ModelReference& model, DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	if (model.isAnimation) {

		dxCommand->TransitionBarriers({ model.animationModel->GetIOVertex().GetResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		commandList->IASetVertexBuffers(0, 1,
			&model.animationModel->GetIOVertex().GetVertexBuffer());
		commandList->IASetIndexBuffer(
			&model.animationModel->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
		indexCount = model.animationModel->GetIA().GetIndexCount(meshIndex);
	} else {

		commandList->IASetVertexBuffers(0, 1,
			&model.model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
		commandList->IASetIndexBuffer(
			&model.model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
		indexCount = model.model->GetIA().GetIndexCount(meshIndex);
	}
}

void MeshCommand::Draw(UINT indexCount, const ModelReference& model, DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	// draw
	commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	if (model.isAnimation) {

		dxCommand->TransitionBarriers({ model.animationModel->GetIOVertex().GetResource() },
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}