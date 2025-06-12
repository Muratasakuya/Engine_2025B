#include "SceneConstBuffer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/SceneView.h>

//============================================================================
//	SceneConstBuffer classMethods
//============================================================================

void SceneConstBuffer::Create(ID3D12Device* device) {

	// buffer作成
	// camera
	viewProjectionBuffer_.CreateConstBuffer(device);
	cameraPosBuffer_.CreateConstBuffer(device);
	orthoProjectionBuffer_.CreateConstBuffer(device);
	// light
	lightBuffer_.CreateConstBuffer(device);

	// rayScene
	raySceneBuffer_.CreateConstBuffer(device);

	// rayScene
	RaySceneForGPU rayScene{};
	rayScene.rayMin = 0.01f;       // 飛ばす最小位置
	rayScene.rayMax = 10000000.0f; // 飛ばす距離
	raySceneBuffer_.TransferData(rayScene);

	// debug
#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	// camera
	debugSceneViewProjectionBuffer_.CreateConstBuffer(device);
	debugSceneCameraPosBuffer_.CreateConstBuffer(device);
#endif
}

void SceneConstBuffer::Update(class SceneView* sceneView) {

	// buffer更新
	// camera
	viewProjectionBuffer_.TransferData(sceneView->GetCamera()->GetViewProjectionMatrix());
	cameraPosBuffer_.TransferData(sceneView->GetCamera()->GetTransform().translation);
	orthoProjectionBuffer_.TransferData(sceneView->GetCamera2D()->GetViewProjectionMatrix());
	// light
	lightBuffer_.TransferData(*sceneView->GetLight());

	// debug
#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	// camera
	debugSceneViewProjectionBuffer_.TransferData(sceneView->GetDebugCamera()->GetViewProjectionMatrix());
	debugSceneCameraPosBuffer_.TransferData(sceneView->GetDebugCamera()->GetTransform().translation);
#endif
}

void SceneConstBuffer::SetMainPassCommands(bool debugEnable, ID3D12GraphicsCommandList* commandList) {

	if (!debugEnable) {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(6,
			viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

		// cameraPos
		commandList->SetGraphicsRootConstantBufferView(14,
			cameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(6,
			debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

		// cameraPos
		commandList->SetGraphicsRootConstantBufferView(14,
			debugSceneCameraPosBuffer_.GetResource()->GetGPUVirtualAddress());
	}
	// lightViewProjection
	commandList->SetGraphicsRootConstantBufferView(7,
		lightViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());

	// light
	commandList->SetGraphicsRootConstantBufferView(13,
		lightBuffer_.GetResource()->GetGPUVirtualAddress());
}

void SceneConstBuffer::SetViewProCommand(bool debugEnable,
	ID3D12GraphicsCommandList* commandList, UINT rootIndex) {

	if (!debugEnable) {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(rootIndex,
			viewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	} else {

		// viewProjection
		commandList->SetGraphicsRootConstantBufferView(rootIndex,
			debugSceneViewProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
	}
}

void SceneConstBuffer::SetOrthoProCommand(ID3D12GraphicsCommandList* commandList, UINT rootIndex) {

	commandList->SetGraphicsRootConstantBufferView(rootIndex,
		orthoProjectionBuffer_.GetResource()->GetGPUVirtualAddress());
}

void SceneConstBuffer::SetRaySceneCommand(ID3D12GraphicsCommandList* commandList, UINT rootIndex) {

	commandList->SetGraphicsRootConstantBufferView(rootIndex, raySceneBuffer_.GetResource()->GetGPUVirtualAddress());
}