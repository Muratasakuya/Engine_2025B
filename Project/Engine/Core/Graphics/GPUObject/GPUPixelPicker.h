#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/GPUObject/DxReadbackBuffer.h>
#include <Engine/Core/Graphics/GPUObject/DxStructuredBuffer.h>
#include <Lib/MathUtils/MathUtils.h>

// front
class SRVDescriptor;
class DxCommand;
class SceneView;

//============================================================================
//	GPUPixelPicker class
//============================================================================
class GPUPixelPicker {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GPUPixelPicker() = default;
	~GPUPixelPicker() = default;

	void Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor);

	void Update(SceneView* sceneView, const Vector2& textureSize, const Vector2& input);

	void Execute(DxCommand* dxCommand, ID3D12Resource* tlasResource);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// ピッキング処理用データ
	struct PickingData {

		uint32_t inputPixelX;
		uint32_t inputPixelY;
		uint32_t textureWidth;
		uint32_t textureHeight;
		Matrix4x4 inverseViewProjection;
		Vector3 cameraWorldPos;
		float rayMax;
	};

	//--------- variables ----------------------------------------------------

	std::unique_ptr<PipelineState> pipeline_;

	// ピッキング処理で取得した情報
	uint32_t pickedID_;

	// buffers
	DxStructuredBuffer<uint32_t> outputIDBuffer_;
	DxConstBuffer<PickingData> pickingBuffer_;
	DxReadbackBuffer<uint32_t> readbackIDBuffer_;
	PickingData pickingData_;

	//--------- functions ----------------------------------------------------

	// helper
	void CopyReadbackResource(DxCommand* dxCommand);
};