#include "BloomProcessor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Context/PostProcessCommandContext.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessPipeline.h>

// imgui
#include <imgui.h>

//============================================================================
//	BloomProcessor classMethods
//============================================================================

void BloomProcessor::Init(ID3D12Device* device, SRVDescriptor* srvDescriptor,
	uint32_t width, uint32_t height) {

	//  輝度抽出用
	luminanceProcess_ = std::make_unique<ComputePostProcessor>();
	luminanceProcess_->Init(device, srvDescriptor, width, height);
	luminanceExtractBuffer_ = std::make_unique<PostProcessBuffer<LuminanceExtractForGPU>>();
	luminanceExtractBuffer_->Init(device, 2);
	// 水平ブラー用
	horizontalBlur_ = std::make_unique<ComputePostProcessor>();
	horizontalBlur_->Init(device, srvDescriptor, width, height);
	horizontalBlurBuffer_ = std::make_unique<PostProcessBuffer<HorizonBlurForGPU>>();
	horizontalBlurBuffer_->Init(device, 2);
	// 垂直ブラー用
	verticalBlur_ = std::make_unique<ComputePostProcessor>();
	verticalBlur_->Init(device, srvDescriptor, width, height);
	verticalBlurBuffer_ = std::make_unique<PostProcessBuffer<VerticalBlurForGPU>>();
	verticalBlurBuffer_->Init(device, 2);
	// 加算合成用
	textureCombine_ = std::make_unique<ComputePostProcessor>();
	textureCombine_->Init(device, srvDescriptor, width, height);
	// 予め垂直ブラー用をかけた後のGPUHandleを取得しておく
	textureCombine_->SetProcessTexureGPUHandle(verticalBlur_->GetSRVGPUHandle());

	// 初期化値設定
	LuminanceExtractForGPU luminance{};
	luminance.threshold = 1.0f; // 輝度抽出閾値
	luminanceExtractBuffer_->SetParameter(&luminance, sizeof(LuminanceExtractForGPU));

	HorizonBlurForGPU horizontalBlur{};
	horizontalBlur.radius = 32;  // 半径
	horizontalBlur.sigma = 8.0f; // ブラー強度
	horizontalBlurBuffer_->SetParameter(&horizontalBlur, sizeof(HorizonBlurForGPU));

	VerticalBlurForGPU verticalBlur{};
	verticalBlur.radius = 32;  // 半径
	verticalBlur.sigma = 8.0f; // ブラー強度
	verticalBlurBuffer_->SetParameter(&verticalBlur, sizeof(VerticalBlurForGPU));
}

void BloomProcessor::Execute(
	DxCommand* dxCommand, PostProcessPipeline* pipeline,
	const D3D12_GPU_DESCRIPTOR_HANDLE& inputGPUHandle) {

	// bufferの更新
	luminanceExtractBuffer_->Update();
	horizontalBlurBuffer_->Update();
	verticalBlurBuffer_->Update();

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);
	PostProcessCommandContext commandContext{};

	// 輝度抽出を行う
	{
		// pipeline設定
		pipeline->SetPipeline(commandList, PostProcessType::BloomLuminanceExtract);

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

	// 輝度抽出を行ったtextureに水平ブラーをかける
	{
		// pipeline設定
		pipeline->SetPipeline(commandList, PostProcessType::HorizontalBlur);

		commandList->SetComputeRootConstantBufferView(
			horizontalBlurBuffer_->GetRootIndex(),
			horizontalBlurBuffer_->GetResource()->GetGPUVirtualAddress());

		// 実行
		commandContext.Execute(
			PostProcessType::HorizontalBlur, commandList,
			horizontalBlur_.get(), luminanceProcess_->GetSRVGPUHandle());

		// UnorderedAccess -> ComputeShader
		dxCommand->TransitionBarriers({ horizontalBlur_->GetOutputTextureResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	// 水平ブラーを行ったtextureに垂直ブラーをかける
	{
		// pipeline設定
		pipeline->SetPipeline(commandList, PostProcessType::VerticalBlur);

		commandList->SetComputeRootConstantBufferView(
			verticalBlurBuffer_->GetRootIndex(),
			verticalBlurBuffer_->GetResource()->GetGPUVirtualAddress());

		// 実行
		commandContext.Execute(
			PostProcessType::VerticalBlur, commandList,
			verticalBlur_.get(), horizontalBlur_->GetSRVGPUHandle());

		// UnorderedAccess -> ComputeShader
		dxCommand->TransitionBarriers({ verticalBlur_->GetOutputTextureResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	// 水平ブラーを行ったtextureに垂直ブラーをかける
	{
		// pipeline設定
		pipeline->SetPipeline(commandList, PostProcessType::BloomCombine);

		// 実行
		commandContext.Execute(
			PostProcessType::BloomCombine, commandList,
			textureCombine_.get(), inputGPUHandle);

		// UnorderedAccess -> PixelShader
		dxCommand->TransitionBarriers({ textureCombine_->GetOutputTextureResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
}

void BloomProcessor::ToWrite(DxCommand* dxCommand) {

	// ComputeShader -> UnorderedAccess
	dxCommand->TransitionBarriers(
		{ luminanceProcess_->GetOutputTextureResource(),
		horizontalBlur_->GetOutputTextureResource(),
		verticalBlur_->GetOutputTextureResource() },
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// PixelShader -> UnorderedAccess
	dxCommand->TransitionBarriers({ textureCombine_->GetOutputTextureResource() },
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void BloomProcessor::ImGui() {

	ImGui::SeparatorText("Bloom");

	luminanceExtractBuffer_->ImGuiWithBloom();
	horizontalBlurBuffer_->ImGuiWithBloom();
	verticalBlurBuffer_->ImGuiWithBloom();
}