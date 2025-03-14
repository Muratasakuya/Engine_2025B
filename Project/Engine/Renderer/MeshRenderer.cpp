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

	// 描画情報取得
	const auto& object3DBuffers = renderObjectManager_->GetObject3DBuffers();

	if (object3DBuffers.empty()) {
		return;
	}
	for (const auto& object : object3DBuffers) {

		// shadowMapへの描画処理
		pipeline_->SetZPassPipeline();

		// lightViewProjection: root1
		commandList_->SetGraphicsRootConstantBufferView(1, lightViewProjectionBuffer_.GetResourceAdress());
		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			if (object.model.isAnimation) {

				dxCommand_->TransitionBarriers({ object.model.animationModel->GetIOVertex().GetResource() },
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				commandList_->IASetVertexBuffers(0, 1,
					&object.model.animationModel->GetIOVertex().GetVertexBuffer());
				commandList_->IASetIndexBuffer(
					&object.model.animationModel->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
				indexCount = object.model.animationModel->GetIA().GetIndexCount(meshIndex);
			} else {

				commandList_->IASetVertexBuffers(0, 1,
					&object.model.model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
				commandList_->IASetIndexBuffer(
					&object.model.model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
				indexCount = object.model.model->GetIA().GetIndexCount(meshIndex);
			}

			// transform: root0
			commandList_->SetGraphicsRootConstantBufferView(0, object.matrix.GetResourceAdress());
			// draw
			commandList_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

			if (object.model.isAnimation) {

				dxCommand_->TransitionBarriers({ object.model.animationModel->GetIOVertex().GetResource() },
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			}
		}
	}
}

void MeshRenderer::Render(bool debugEnable) {

	// line描画実行
	LineRenderer::GetInstance()->ExecuteLine(debugEnable);

	// 描画情報取得
	const auto& object3DBuffers = renderObjectManager_->GetObject3DBuffers();

	if (object3DBuffers.empty()) {
		return;
	}
	for (const auto& object : object3DBuffers) {

		// renderTextureへの描画処理
		pipeline_->SetObjectPipeline();

		// shadowMap: root1
		commandList_->SetGraphicsRootDescriptorTable(1, shadowMap_->GetGPUHandle());
		if (!debugEnable) {

			// viewProjection: root3
			commandList_->SetGraphicsRootConstantBufferView(3, viewProjectionBuffer_.GetResourceAdress());
			// camera:         root7
			commandList_->SetGraphicsRootConstantBufferView(7, cameraPosBuffer_.GetResourceAdress());
		} else {

			// viewProjection: root3
			commandList_->SetGraphicsRootConstantBufferView(3, debugSceneViewProjectionBuffer_.GetResourceAdress());
			// camera:         root7
			commandList_->SetGraphicsRootConstantBufferView(7, debugSceneCameraPosBuffer_.GetResourceAdress());
		}
		// lightViewProjection: root4
		commandList_->SetGraphicsRootConstantBufferView(4, lightViewProjectionBuffer_.GetResourceAdress());
		// light:  root6
		commandList_->SetGraphicsRootConstantBufferView(6, lightBuffer_.GetResourceAdress());

		uint32_t meshNum = 0;
		if (object.model.isAnimation) {

			meshNum = 1;
		} else {

			meshNum = static_cast<uint32_t>(object.model.model->GetMeshNum());
		}
		for (uint32_t meshIndex = 0; meshIndex < meshNum; ++meshIndex) {

			UINT indexCount = 0;
			if (object.model.isAnimation) {

				dxCommand_->TransitionBarriers({ object.model.animationModel->GetIOVertex().GetResource() },
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

				commandList_->IASetVertexBuffers(0, 1,
					&object.model.animationModel->GetIOVertex().GetVertexBuffer());
				commandList_->IASetIndexBuffer(
					&object.model.animationModel->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
				indexCount = object.model.animationModel->GetIA().GetIndexCount(meshIndex);

				// texture: root0
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.animationModel->GetTextureGPUHandle(meshIndex));
			} else {

				commandList_->IASetVertexBuffers(0, 1,
					&object.model.model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
				commandList_->IASetIndexBuffer(
					&object.model.model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
				indexCount = object.model.model->GetIA().GetIndexCount(meshIndex);

				// texture: root0
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.model->GetTextureGPUHandle(meshIndex));
			}

			// transform: root2
			commandList_->SetGraphicsRootConstantBufferView(2, object.matrix.GetResourceAdress());
			// material:  root3
			commandList_->SetGraphicsRootConstantBufferView(5, object.materials[meshIndex].GetResourceAdress());
			// draw
			commandList_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

			if (object.model.isAnimation) {

				dxCommand_->TransitionBarriers({ object.model.animationModel->GetIOVertex().GetResource() },
					D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			}
		}
	}
}