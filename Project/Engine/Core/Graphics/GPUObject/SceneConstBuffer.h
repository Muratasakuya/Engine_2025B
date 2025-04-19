#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
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

	void Update(class CameraManager* cameraManager,
		class LightManager* lightManager);

	void SetZPassCommands(ID3D12GraphicsCommandList* commandList);
	void SetMainPassCommands(bool debugEnable, ID3D12GraphicsCommandList* commandList);
	void SetViewProCommand(bool debugEnable, ID3D12GraphicsCommandList* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// camera
	// main
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
	DxConstBuffer<Matrix4x4> lightViewProjectionBuffer_;
	DxConstBuffer<Vector3> cameraPosBuffer_;
	// debug
	DxConstBuffer<Matrix4x4> debugSceneViewProjectionBuffer_;
	DxConstBuffer<Vector3> debugSceneCameraPosBuffer_;

	// light
	DxConstBuffer<PunctualLight> lightBuffer_;

	//--------- functions ----------------------------------------------------

};