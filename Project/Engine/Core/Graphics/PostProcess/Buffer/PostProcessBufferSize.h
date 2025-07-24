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

class BloomForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BloomForGPU() = default;
	~BloomForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float threshold = 1.0f;
	int radius = 2;
	float sigma = 32.0f;
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

	void SetProjectionInverse(const Matrix4x4& projection) { projectionInverse = projection; }

	//--------- properties ---------------------------------------------------

	Matrix4x4 projectionInverse;

	float edgeScale = 1.0f;
	float threshold = 12.0f;
	float pad0[2];

	Vector3 color = Vector3::AnyInit(0.0f);
	float pad1;
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

	float scale = 24.0f;
	float power = 0.0f;
	float padding[2];
	Vector3 color = Vector3::AnyInit(1.0f);
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

	Vector2 center = Vector2::AnyInit(0.5f);
	int numSamples = 8;
	float width = 0.01f;
};

class HorizonBlurForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	HorizonBlurForGPU() = default;
	~HorizonBlurForGPU() = default;

	void ImGui();

	void ImGuiWithBloom();

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

	void ImGuiWithBloom();

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

	void ImGuiWithBloom();

	//--------- properties ---------------------------------------------------

	float threshold;
};

class LutForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LutForGPU() = default;
	~LutForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float lerpRate = 0.2f;
	float lutSize = 33.0f;
};

class RandomForGPU {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RandomForGPU() = default;
	~RandomForGPU() = default;

	void ImGui();

	//--------- properties ---------------------------------------------------

	float time = 0.001f;
};