#include "BloomProcessor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessCommandContext.h>

//============================================================================
//	BloomProcessor classMethods
//============================================================================

void BloomProcessor::Init(ID3D12Device* device, SRVManager* srvManager,
	uint32_t width, uint32_t height) {

	//  輝度抽出用
	luminanceProcess_ = std::make_unique<ComputePostProcessor>();
	luminanceProcess_->Init(device, srvManager, width, height);
	luminanceExtractBuffer_ = std::make_unique<PostProcessBuffer<LuminanceExtractForGPU>>();
	luminanceExtractBuffer_->Init(device, 2);
	// 水平ブラー用
	horizontalBlur_ = std::make_unique<ComputePostProcessor>();
	horizontalBlur_->Init(device, srvManager, width, height);
	horizontalBlurBuffer_ = std::make_unique<PostProcessBuffer<HorizonBlurForGPU>>();
	horizontalBlurBuffer_->Init(device, 2);
	// 垂直ブラー用
	verticalBlur_ = std::make_unique<ComputePostProcessor>();
	verticalBlur_->Init(device, srvManager, width, height);
	verticalBlurBuffer_ = std::make_unique<PostProcessBuffer<VerticalBlurForGPU>>();
	verticalBlurBuffer_->Init(device, 2);
	// 加算合成用
	textureCombine_ = std::make_unique<ComputePostProcessor>();
	textureCombine_->Init(device, srvManager, width, height);
	// 予め垂直ブラー用をかけた後のGPUHandleを取得しておく
	textureCombine_->SetProcessTexureGPUHandle(verticalBlur_->GetSRVGPUHandle());

	// 初期化値設定
	LuminanceExtractForGPU luminance{};
	luminance.threshold = 1.0f; // 輝度抽出閾値
	luminanceExtractBuffer_->SetProperties(luminance);

	HorizonBlurForGPU horizontalBlur{};
	horizontalBlur.radius = 32;  // 半径
	horizontalBlur.sigma = 8.0f; // ブラー強度
	horizontalBlurBuffer_->SetProperties(horizontalBlur);

	VerticalBlurForGPU verticalBlur{};
	verticalBlur.radius = 32;  // 半径
	verticalBlur.sigma = 8.0f; // ブラー強度
	verticalBlurBuffer_->SetProperties(verticalBlur);
}

void BloomProcessor::Execute(DxCommand* dxCommand, const D3D12_GPU_DESCRIPTOR_HANDLE& inputGPUHandle) {

	// bufferの更新
	luminanceExtractBuffer_->Update();
	horizontalBlurBuffer_->Update();
	verticalBlurBuffer_->Update();

	auto commandList = dxCommand->GetCommandList(CommandListType::Compute);
	PostProcessCommandContext commandContext{};

	// 輝度抽出を行う
	{
		// pipeline設定

		commandList->SetComputeRootConstantBufferView(
			luminanceExtractBuffer_->GetRootIndex(),
			luminanceExtractBuffer_->GetResource()->GetGPUVirtualAddress());

		// 実行
		commandContext.Execute(
			PostProcessType::BloomLuminanceExtract, commandList,
			luminanceProcess_.get(), inputGPUHandle);

		// UnorderedAccess -> ComputeShader
		dxCommand->TransitionBarriers({ luminanceProcess_->GetOutputTextureResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}
}