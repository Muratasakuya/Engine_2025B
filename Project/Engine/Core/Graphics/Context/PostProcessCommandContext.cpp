#include "PostProcessCommandContext.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/ComputePostProcessor.h>
#include <Engine/Core/Graphics/Lib/DxUtils.h>

//============================================================================
//	PostProcessCommandContext classMethods
//============================================================================

void PostProcessCommandContext::Execute(PostProcessType type,
	ID3D12GraphicsCommandList* commandList, ComputePostProcessor* processor,
	const D3D12_GPU_DESCRIPTOR_HANDLE& inputTextureGPUHandle) {

	const UINT kThreadGroupSize = 8;
	UINT threadGroupCountX = DxUtils::RoundUp(static_cast<UINT>(processor->GetTextureSize().x), kThreadGroupSize);
	UINT threadGroupCountY = DxUtils::RoundUp(static_cast<UINT>(processor->GetTextureSize().y), kThreadGroupSize);

	// typeごとに処理
	switch (type) {
	case PostProcessType::Random:
	case PostProcessType::Bloom:
	case PostProcessType::Vignette:
	case PostProcessType::Grayscale:
	case PostProcessType::SepiaTone:
	case PostProcessType::BoxFilter:
	case PostProcessType::RadialBlur:
	case PostProcessType::VerticalBlur:
	case PostProcessType::GaussianFilter:
	case PostProcessType::HorizontalBlur:
	case PostProcessType::LuminanceBasedOutline:

		commandList->SetComputeRootDescriptorTable(0, processor->GetUAVGPUHandle());
		commandList->SetComputeRootDescriptorTable(1, inputTextureGPUHandle);
		commandList->Dispatch(threadGroupCountX, threadGroupCountY, 1);
		break;
	case PostProcessType::Dissolve:
	case PostProcessType::DepthBasedOutline:
	case PostProcessType::Lut:
	case PostProcessType::Glitch:

		commandList->SetComputeRootDescriptorTable(0, processor->GetUAVGPUHandle());
		commandList->SetComputeRootDescriptorTable(1, inputTextureGPUHandle);
		commandList->SetComputeRootDescriptorTable(2, processor->GetProcessTextureGPUHandle());
		commandList->Dispatch(threadGroupCountX, threadGroupCountY, 1);
		break;
	}
}