#pragma once

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