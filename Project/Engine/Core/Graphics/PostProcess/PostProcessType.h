#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>

//============================================================================
//	PostProcessType enum class
//============================================================================

// postProcessの種類
enum class PostProcessType {

	HorizontalBlur,
	VerticalBlur,
	RadialBlur,
	GaussianFilter,
	BoxFilter,
	Dissolve,
	Random,
	Vignette,
	Grayscale,
	SepiaTone,
	BloomLuminanceExtract,
	BloomCombine,
	LuminanceBasedOutline,
	DepthBasedOutline,

	Count
};
// userが使う種類
enum class PostProcess {

	Bloom,
	HorizontalBlur,
	VerticalBlur,
	RadialBlur,
	GaussianFilter,
	BoxFilter,
	Dissolve,
	Random,
	Vignette,
	Grayscale,
	SepiaTone,
	LuminanceBasedOutline,
	DepthBasedOutline,

	Count
};
static constexpr uint32_t kPostProcessCount = static_cast<uint32_t>(PostProcessType::Count);