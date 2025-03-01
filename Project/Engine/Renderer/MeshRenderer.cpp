#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/ShadowMap.h>
#include <Engine/Component/EntityComponent.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	MeshRenderer classMethods
//============================================================================

void MeshRenderer::Init(DxCommand* dxCommand, ID3D12Device* device,
	ShadowMap* shadowMap, DxShaderCompiler* shaderCompiler,
	CameraManager* cameraManager) {

	commandList_ = nullptr;
	commandList_ = dxCommand->GetCommandList(CommandListType::Graphics);

	dxCommand_ = nullptr;
	dxCommand_ = dxCommand;

	shadowMap_ = nullptr;
	shadowMap_ = shadowMap;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	entityComponent_ = nullptr;
	entityComponent_ = EntityComponent::GetInstance();

	pipeline_ = std::make_unique<ObjectPipelineManager>();
	pipeline_->Create(commandList_, device, shaderCompiler);

	viewProjectionBuffer_.CreateConstBuffer(device);
	lightViewProjectionBuffer_.CreateConstBuffer(device);
	cameraPosBuffer_.CreateConstBuffer(device);
	light_.Init();
	lightBuffer_.CreateConstBuffer(device);
}

void MeshRenderer::Update() {

	viewProjectionBuffer_.TransferData(cameraManager_->GetCamera()->GetViewProjectionMatrix());
	lightViewProjectionBuffer_.TransferData(cameraManager_->GetLightViewCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(cameraManager_->GetCamera()->GetTransform().translation);
	lightBuffer_.TransferData(light_);
}

void MeshRenderer::RenderZPass() {

	const auto& sortedEntities = entityComponent_->GetBuffers();
	if (sortedEntities.empty()) {
		return;
	}
	for (auto& [blendMode, entityList] : sortedEntities) {

		// shadowMapへの描画処理
		pipeline_->SetZPassPipeline();

		// lightViewProjection: root1
		commandList_->SetGraphicsRootConstantBufferView(1, lightViewProjectionBuffer_.GetResourceAdress());

		for (auto& entity : entityList) {
			std::visit([&](auto& buffer) {
				for (uint32_t meshIndex = 0; meshIndex < buffer->model->GetMeshNum(); ++meshIndex) {

					// IA
					if constexpr (std::is_same_v<std::decay_t<decltype(buffer)>, EntityAnimationBufferData*>) {

						dxCommand_->TransitionBarriers({ buffer->model->GetIOVertex().GetResource() },
							D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
							D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

						commandList_->IASetVertexBuffers(0, 1,
							&buffer->model->GetIOVertex().GetVertexBuffer());
					} else {
						commandList_->IASetVertexBuffers(0, 1,
							&buffer->model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
					}
					commandList_->IASetIndexBuffer(
						&buffer->model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());

					// transform: root0
					commandList_->SetGraphicsRootConstantBufferView(0, buffer->transform.GetResourceAdress());
					// draw
					commandList_->DrawIndexedInstanced(buffer->model->GetIA().GetIndexCount(meshIndex), 1, 0, 0, 0);

					if constexpr (std::is_same_v<std::decay_t<decltype(buffer)>, EntityAnimationBufferData*>) {

						dxCommand_->TransitionBarriers({ buffer->model->GetIOVertex().GetResource() },
							D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
							D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
					}
				}
				}, entity);
		}
	}
}

void MeshRenderer::Render() {

	const auto& sortedEntities = entityComponent_->GetBuffers();
	if (sortedEntities.empty()) {
		return;
	}
	for (auto& [blendMode, entityList] : sortedEntities) {

		// renderTextureへの描画処理
		pipeline_->SetObjectPipeline(blendMode);

		// shadowMap: root1
		commandList_->SetGraphicsRootDescriptorTable(1, shadowMap_->GetGPUHandle());
		// viewProjection:      root3
		commandList_->SetGraphicsRootConstantBufferView(3, viewProjectionBuffer_.GetResourceAdress());
		// lightViewProjection: root4
		commandList_->SetGraphicsRootConstantBufferView(4, lightViewProjectionBuffer_.GetResourceAdress());
		// light:  root6
		commandList_->SetGraphicsRootConstantBufferView(6, lightBuffer_.GetResourceAdress());
		// camera: root7
		commandList_->SetGraphicsRootConstantBufferView(7, cameraPosBuffer_.GetResourceAdress());

		for (auto& entity : entityList) {
			std::visit([&](auto& buffer) {
				for (uint32_t meshIndex = 0; meshIndex < buffer->model->GetMeshNum(); ++meshIndex) {

					// IA
					if constexpr (std::is_same_v<std::decay_t<decltype(buffer)>, EntityAnimationBufferData*>) {

						dxCommand_->TransitionBarriers({ buffer->model->GetIOVertex().GetResource() },
							D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
							D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

						commandList_->IASetVertexBuffers(0, 1,
							&buffer->model->GetIOVertex().GetVertexBuffer());
					} else {
						commandList_->IASetVertexBuffers(0, 1,
							&buffer->model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
					}
					commandList_->IASetIndexBuffer(
						&buffer->model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());

					// texture:   root0
					commandList_->SetGraphicsRootDescriptorTable(0, buffer->model->GetTextureGPUHandle(meshIndex));
					// transform: root2
					commandList_->SetGraphicsRootConstantBufferView(2, buffer->transform.GetResourceAdress());
					// material:  root3
					commandList_->SetGraphicsRootConstantBufferView(5, buffer->materials[meshIndex].GetResourceAdress());
					// draw
					commandList_->DrawIndexedInstanced(buffer->model->GetIA().GetIndexCount(meshIndex), 1, 0, 0, 0);

					if constexpr (std::is_same_v<std::decay_t<decltype(buffer)>, EntityAnimationBufferData*>) {

						dxCommand_->TransitionBarriers({ buffer->model->GetIOVertex().GetResource() },
							D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
							D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
					}
				}
				}, entity);
		}
	}
}