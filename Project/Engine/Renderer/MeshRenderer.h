#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
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
class SRVManager;
class ShadowMap;
class GPUObjectSystem;

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

	void Init(ID3D12Device8* device, ShadowMap* shadowMap,
		DxShaderCompiler* shaderCompiler, SRVManager* srvManager);

	void Update(class CameraManager* cameraManager);

	void RenderingZPass(GPUObjectSystem* gpuObjectSystem,
		ID3D12GraphicsCommandList6* commandList);

	void Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
		ID3D12GraphicsCommandList6* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SRVManager* srvManager_;
	ShadowMap* shadowMap_;

	std::unique_ptr<PipelineState> meshShaderPipeline_;
	std::unique_ptr<PipelineState> meshShaderZPassPipeline_;

	// buffer
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> lightViewProjectionBuffer_;
	DxConstBuffer<Vector3> cameraPosBuffer_;

	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	DxConstBuffer<Vector3> debugSceneCameraPosBuffer_;

	PunctualLight light_;
	DxConstBuffer<PunctualLight> lightBuffer_;

	//--------- functions ----------------------------------------------------

	// 共通のbuffer
	void SetCommonBuffer(bool debugEnable, ID3D12GraphicsCommandList6* commandList);
};