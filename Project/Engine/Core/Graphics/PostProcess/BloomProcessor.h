#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/ComputePostProcessor.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBuffer.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>

// c++
#include <memory>

//============================================================================
//	BloomProcessor class
//============================================================================
class BloomProcessor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BloomProcessor() = default;
	~BloomProcessor() = default;

	void Init(ID3D12Device* device, class SRVDescriptor* srvDescriptor,
		uint32_t width, uint32_t height);

	void Execute(class DxCommand* dxCommand,
		class PostProcessPipeline* pipeline,
		const D3D12_GPU_DESCRIPTOR_HANDLE& inputGPUHandle);

	void ToWrite(class DxCommand* dxCommand);

	void ImGui();

	//--------- accessor -----------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return textureCombine_->GetSRVGPUHandle(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 輝度抽出用
	std::unique_ptr<ComputePostProcessor> luminanceProcess_;
	std::unique_ptr<PostProcessBuffer<LuminanceExtractForGPU>> luminanceExtractBuffer_;

	// ブラー用
	std::unique_ptr<ComputePostProcessor> horizontalBlur_; // 水平ブラー
	std::unique_ptr<PostProcessBuffer<HorizonBlurForGPU>> horizontalBlurBuffer_;
	std::unique_ptr<ComputePostProcessor> verticalBlur_;   // 垂直ブラー
	std::unique_ptr<PostProcessBuffer<VerticalBlurForGPU>> verticalBlurBuffer_;

	// 加算合成用
	std::unique_ptr<ComputePostProcessor> textureCombine_;
};