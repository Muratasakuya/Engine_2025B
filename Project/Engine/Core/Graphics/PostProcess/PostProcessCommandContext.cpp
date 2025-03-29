#include "PostProcessCommandContext.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/ComputePostProcessor.h>

//============================================================================
//	PostProcessCommandContext classMethods
//============================================================================

void PostProcessCommandContext::Execute(PostProcessType type,
	ID3D12GraphicsCommandList* commandList, ComputePostProcessor* processor,
	const D3D12_GPU_DESCRIPTOR_HANDLE& inputTextureGPUHandle) {

	// typeごとに処理
	switch (type) {
	case PostProcessType::Random:
	case PostProcessType::Vignette:
	case PostProcessType::Grayscale:
	case PostProcessType::SepiaTone:
	case PostProcessType::BoxFilter:
	case PostProcessType::RadialBlur:
	case PostProcessType::VerticalBlur:
	case PostProcessType::GaussianFilter:
	case PostProcessType::HorizontalBlur:
	case PostProcessType::BloomLuminanceExtract:
	case PostProcessType::LuminanceBasedOutline:

		commandList->SetComputeRootDescriptorTable(0, processor->GetUAVGPUHandle());
		commandList->SetComputeRootDescriptorTable(1, inputTextureGPUHandle);

		UINT threadGroupCountX = static_cast<uint32_t>(processor->GetTextureSize().x + 7) / 8;
		UINT threadGroupCountY = static_cast<uint32_t>(processor->GetTextureSize().y + 7) / 8;
		commandList->Dispatch(threadGroupCountX, threadGroupCountY, 1);
		break;
	case PostProcessType::Dissolve:
	case PostProcessType::BloomCombine:
	case PostProcessType::DepthBasedOutline:

		commandList->SetComputeRootDescriptorTable(0, processor->GetUAVGPUHandle());
		commandList->SetComputeRootDescriptorTable(1, inputTextureGPUHandle);
		commandList->SetComputeRootDescriptorTable(2, processor->GetProcessTextureGPUHandle());

		UINT threadGroupCountX = static_cast<uint32_t>(processor->GetTextureSize().x + 7) / 8;
		UINT threadGroupCountY = static_cast<uint32_t>(processor->GetTextureSize().y + 7) / 8;
		commandList->Dispatch(threadGroupCountX, threadGroupCountY, 1);
		break;
	}
}