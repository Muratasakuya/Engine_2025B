#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Renderer/Managers/ObjectPipelineManager.h>
#include <Engine/Component/ModelComponent.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>
#include <Game/Light/PunctualLight.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Matrix4x4.h>

// directX
#include <d3d12.h>
// c++
#include <memory>
#include <ranges>
// front
class DxCommand;
class DxShaderCompiler;
class ShadowMap;
class RenderObjectManager;
class CameraManager;

//============================================================================
//	MeshRenderer class
//============================================================================
class MeshRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshRenderer() = default;
	~MeshRenderer() = default;

	void Init(DxCommand* dxCommand, ID3D12Device* device,
		ShadowMap* shadowMap, DxShaderCompiler* shaderCompiler,
		RenderObjectManager* renderObjectManager, CameraManager* cameraManager);

	void Update();

	void RenderZPass();
	void Render(bool debugEnable);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	DxCommand* dxCommand_;
	ID3D12GraphicsCommandList* commandList_;
	ShadowMap* shadowMap_;
	RenderObjectManager* renderObjectManager_;
	CameraManager* cameraManager_;

	std::unique_ptr<ObjectPipelineManager> pipeline_;

	//* buffers *//

	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> lightViewProjectionBuffer_;
	DxConstBuffer<Vector3> cameraPosBuffer_;

	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	DxConstBuffer<Vector3> debugSceneCameraPosBuffer_;

	PunctualLight light_;
	DxConstBuffer<PunctualLight> lightBuffer_;
};

//============================================================================
//	MeshCommand namespace
//============================================================================

namespace MeshCommand {

	inline void IA(UINT& indexCount, uint32_t meshIndex,
		const ModelReference& model, DxCommand* dxCommand);

	inline void Draw(UINT indexCount, const ModelReference& model, DxCommand* dxCommand);
}