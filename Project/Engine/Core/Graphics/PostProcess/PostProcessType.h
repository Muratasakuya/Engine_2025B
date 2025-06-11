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
	Lut,

	Count
};
static constexpr uint32_t kPostProcessCount = static_cast<uint32_t>(PostProcessType::Count);