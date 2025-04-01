#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	DerivedBuffer class
//============================================================================

class DissolveForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DissolveForGPU() = default;
	~DissolveForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float threshold;
	float edgeSize;
	float padding1[2];
	Vector3 edgeColor;
	float padding2;
	Vector3 thresholdColor;
	float padding3;
};

class GaussianFilterForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GaussianFilterForGPU() = default;
	~GaussianFilterForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float sigma;
};

class LuminanceBasedOutlineForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LuminanceBasedOutlineForGPU() = default;
	~LuminanceBasedOutlineForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float strength;
};

class DepthBasedOutlineForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DepthBasedOutlineForGPU() = default;
	~DepthBasedOutlineForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	Matrix4x4 projectionInverse;
};

class VignetteForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	VignetteForGPU() = default;
	~VignetteForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float scale;
	float power;
};

class RadialBlurForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RadialBlurForGPU() = default;
	~RadialBlurForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	Vector2 center;
	int numSamples;
	float width;
};

class HorizonBlurForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	HorizonBlurForGPU() = default;
	~HorizonBlurForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	int radius;
	float sigma;
};

class VerticalBlurForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	VerticalBlurForGPU() = default;
	~VerticalBlurForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	int radius;
	float sigma;
};

class LuminanceExtractForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LuminanceExtractForGPU() = default;
	~LuminanceExtractForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float threshold;
};