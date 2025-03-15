#include "MeshRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Mesh/MeshCommandContext.h>
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
	MeshCommandContext commandContext{};

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
			commandContext.IA(indexCount, meshIndex, object.model, dxCommand_);

			object.matrix.SetCommand(commandList_, 0);

			commandContext.Draw(indexCount, object.model, dxCommand_);
		}
	}
}

void MeshRenderer::Render(bool debugEnable) {

	// line描画実行
	LineRenderer::GetInstance()->ExecuteLine(debugEnable);

	// 通常の描画処理
	NormalRendering(debugEnable);
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
			commandContext.IA(indexCount, meshIndex, object.model, dxCommand_);

			if (object.model.isAnimation) {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.animationModel->GetTextureGPUHandle(meshIndex));
			} else {
				commandList_->SetGraphicsRootDescriptorTable(0, object.model.model->GetTextureGPUHandle(meshIndex));
			}
			object.matrix.SetCommand(commandList_, 2);
			object.materials[meshIndex].SetCommand(commandList_);

			commandContext.Draw(indexCount, object.model, dxCommand_);
		}
	}
}

void MeshRenderer::IndirectRendering(bool debugEnable) {

	debugEnable;

	//// 描画情報取得
	//auto object3DBuffers = renderObjectManager_->GetObject3DBuffers();

	//if (object3DBuffers.empty()) {
	//	return;
	//}

	//// renderTextureへの描画処理
	//pipeline_->SetObjectPipeline();

	//// 全object共通のbuffer設定
	//commandList_->SetGraphicsRootDescriptorTable(1, shadowMap_->GetGPUHandle());
	//if (!debugEnable) {

	//	viewProjectionBuffer_.SetCommand(commandList_);
	//	cameraPosBuffer_.SetCommand(commandList_);
	//} else {

	//	debugSceneViewProjectionBuffer_.SetCommand(commandList_);
	//	debugSceneCameraPosBuffer_.SetCommand(commandList_);
	//}
	//lightViewProjectionBuffer_.SetCommand(commandList_, 4);
	//lightBuffer_.SetCommand(commandList_);

	//std::vector<IndirectCommand> commands;
	//UINT accumulatedIndexCount = 0;
	//UINT accumulatedVertexCount = 0;

	//for (auto& object : object3DBuffers) {

	//	IndirectCommand command = {};

	//	// 描画時のindex数
	//	command.drawArguments.IndexCountPerInstance = object.model.model->GetIA().GetIndexCount(0);
	//	command.drawArguments.StartIndexLocation = accumulatedIndexCount;
	//	command.drawArguments.BaseVertexLocation = accumulatedVertexCount;
	//	command.drawArguments.InstanceCount = 1;
	//	command.drawArguments.StartInstanceLocation = 0;

	//	// matrixの設定
	//	command.matrixBufferAddress = object.matrix.GetResource()->GetGPUVirtualAddress();
	//	// materialの設定
	//	command.materialBufferAddress = object.materials[0].GetResource()->GetGPUVirtualAddress();
	//	// textureの設定
	//	if (object.model.isAnimation) {
	//		command.textureDescriptorIndex = object.model.animationModel->GetTextureGPUIndex(0);
	//	} else {
	//		command.textureDescriptorIndex = object.model.model->GetTextureGPUIndex(0);
	//	}

	//	commands.emplace_back(command);

	//	accumulatedIndexCount += command.drawArguments.IndexCountPerInstance;
	//	accumulatedVertexCount += object.model.model->GetIA().GetVertexCount(0);
	//}

	//indirectCommand_->Update(commands);

	//// 描画処理実行
	//indirectCommand_->Execute(dxCommand_, static_cast<UINT>(commands.size()));
}