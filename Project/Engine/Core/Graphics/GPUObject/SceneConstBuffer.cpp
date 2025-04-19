#include "SceneConstBuffer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>

//============================================================================
//	SceneConstBuffer classMethods
//============================================================================

void SceneConstBuffer::Create(ID3D12Device* device) {

	// buffer作成
	// camera
	viewProjectionBuffer_.CreateConstBuffer(device);
	lightViewProjectionBuffer_.CreateConstBuffer(device);
	cameraPosBuffer_.CreateConstBuffer(device);
	// light
	lightBuffer_.CreateConstBuffer(device);

	// debug
#ifdef _DEBUG

	// camera
	debugSceneViewProjectionBuffer_.CreateConstBuffer(device);
	debugSceneCameraPosBuffer_.CreateConstBuffer(device);
#endif // _DEBUG
}

void SceneConstBuffer::Update(CameraManager* cameraManager,
	LightManager* lightManager) {

	// buffer更新
	// camera
	viewProjectionBuffer_.TransferData(cameraManager->GetCamera()->GetViewProjectionMatrix());
	lightViewProjectionBuffer_.TransferData(cameraManager->GetLightViewCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(cameraManager->GetCamera()->GetTransform().translation);
	// light
	lightBuffer_.TransferData(*lightManager->GetLight());

	// debug
#ifdef _DEBUG

	// camera
	debugSceneViewProjectionBuffer_.TransferData(cameraManager->GetDebugCamera()->GetViewProjectionMatrix());
	debugSceneCameraPosBuffer_.TransferData(cameraManager->GetDebugCamera()->GetTransform().translation);
#endif // _DEBUG
}

void SceneConstBuffer::SetZPassCommands(ID3D12GraphicsCommandList* commandList) {

	// lightViewProjection
	commandList->SetGraphicsRootConstantBufferView(6,
		lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
}

void SceneConstBuffer::SetMainPassCommands(bool debugEnable, ID3D12GraphicsCommandList* commandList) {

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

	// light
	commandList->SetGraphicsRootConstantBufferView(11,
		lightBuffer_.GetResource()->GetGPUVirtualAddress());
}

void SceneConstBuffer::SetViewProCommand(bool debugEnable, ID3D12GraphicsCommandList* commandList) {

	if (!debugEnable) {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(5,
			viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(5,
			debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	}
}