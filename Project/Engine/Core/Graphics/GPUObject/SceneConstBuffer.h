#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingStructures.h>
#include <Engine/Core/Graphics/GPUObject/DitherStructures.h>
#include <Engine/Scene/Light/PunctualLight.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Matrix4x4.h>

//============================================================================
//	SceneConstBuffer class
//============================================================================
class SceneConstBuffer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneConstBuffer() = default;
	~SceneConstBuffer() = default;

	void Create(ID3D12Device* device);

	void Update(class SceneView* sceneView);

	void SetMainPassCommands(bool debugEnable, ID3D12GraphicsCommandList* commandList);
	void SetViewProCommand(bool debugEnable, ID3D12GraphicsCommandList* commandList, UINT rootIndex);
	void SetPerViewCommand(bool debugEnable, ID3D12GraphicsCommandList* commandList, UINT rootIndex);
	void SetOrthoProCommand(ID3D12GraphicsCommandList* commandList, UINT rootIndex);
	void SetRaySceneCommand(ID3D12GraphicsCommandList* commandList, UINT rootIndex);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structres ----------------------------------------------------

	struct PerViewForGPU {

		Matrix4x4 viewProjection;
		Matrix4x4 billboardMatrix;
	};

	//--------- variables ----------------------------------------------------

	// camera3D
	// main
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Vector3> cameraPosBuffer_;
	// debug
	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	DxConstBuffer<Vector3> debugSceneCameraPosBuffer_;
	// camera2D
	DxConstBuffer<Matrix4x4> orthoProjectionBuffer_;

	// light
	DxConstBuffer<PunctualLight> lightBuffer_;

	// rayScene
	DxConstBuffer<RaySceneForGPU> raySceneBuffer_;

	// dither
	DxConstBuffer<DitherForGPU> ditherBuffer_;

	// perView
	DxConstBuffer<PerViewForGPU> perViewBuffer_;
	DxConstBuffer<PerViewForGPU> debugScenePerViewBuffer_;
};