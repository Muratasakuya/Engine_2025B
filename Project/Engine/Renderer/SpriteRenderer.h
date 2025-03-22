#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/CBuffer/DxConstBuffer.h>
#include <Lib/MathUtils/Matrix4x4.h>

// c++
#include <array>
#include <memory>
// front
class DxShaderCompiler;
class RenderObjectManager;
class CameraManager;

//============================================================================
//	SpriteRenderer enum class
//============================================================================

// 描画モード
enum class RenderMode {

	IrrelevantPostProcess,
	ApplyPostProcess,

	Count
};

//============================================================================
//	SpriteRenderer class
//============================================================================
class SpriteRenderer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteRenderer() = default;
	~SpriteRenderer() = default;

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
		DxShaderCompiler* shaderCompiler, RenderObjectManager* renderObjectManager,
		CameraManager* cameraManager);

	void Update();

	void Render(RenderMode renderMode);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12GraphicsCommandList* commandList_;
	RenderObjectManager* renderObjectManager_;
	CameraManager* cameraManager_;

	static constexpr uint32_t kModeCount = static_cast<uint32_t>(RenderMode::Count);

	std::array<std::unique_ptr<PipelineState>, kModeCount> pipelines_;

	// buffer
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;

	//--------- functions ----------------------------------------------------

};