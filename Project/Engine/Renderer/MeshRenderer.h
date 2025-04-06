#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/MeshShaderPipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
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
class SRVManager;
class ShadowMap;
class GPUObjectSystem;
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

	void Init(DxCommand* dxCommand, ID3D12Device8* device,
		ShadowMap* shadowMap, DxShaderCompiler* shaderCompiler, SRVManager* srvManager,
		GPUObjectSystem* gpuObjectSystem, CameraManager* cameraManager);

	void Update();

	void ZPassRendering();
	void Rendering(bool debugEnable, ID3D12GraphicsCommandList6* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	DxCommand* dxCommand_;
	ID3D12GraphicsCommandList* commandList_;
	SRVManager* srvManager_;
	ShadowMap* shadowMap_;
	GPUObjectSystem* gpuObjectSystem_;
	CameraManager* cameraManager_;

	std::unique_ptr<MeshShaderPipelineState> meshShaderPipeline_;

	// buffer
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> lightViewProjectionBuffer_;
	DxConstBuffer<Vector3> cameraPosBuffer_;

	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	DxConstBuffer<Vector3> debugSceneCameraPosBuffer_;

	PunctualLight light_;
	DxConstBuffer<PunctualLight> lightBuffer_;
};