#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Component/SpriteComponent.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Lib/MathUtils/Matrix4x4.h>

// c++
#include <array>
#include <memory>

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

	void Init(ID3D12Device8* device, class SRVDescriptor* srvDescriptor,
		class DxShaderCompiler* shaderCompiler);

	void Update(class CameraManager* cameraManager);

	void RenderIrrelevant(class GPUObjectSystem* gpuObjectSystem,
		ID3D12GraphicsCommandList6* commandList);
	void RenderApply(SpriteLayer layer, class GPUObjectSystem* gpuObjectSystem,
		ID3D12GraphicsCommandList6* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- enum class ---------------------------------------------------

	// 描画モード
	enum class RenderMode {

		IrrelevantPostProcess,
		ApplyPostProcess,

		Count
	};

	//--------- variables ----------------------------------------------------

	static constexpr uint32_t kModeCount = static_cast<uint32_t>(RenderMode::Count);

	std::array<std::unique_ptr<PipelineState>, kModeCount> pipelines_;

	// buffer
	DxConstBuffer<Matrix4x4> viewProjectionBuffer_;
};