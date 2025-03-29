#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	IPostProcessBufferSize class
//============================================================================
class IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IPostProcessBufferSize() = default;
	virtual ~IPostProcessBufferSize() = default;

	virtual void ImGui() = 0;

protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemWidth_ = 192.0f;
};

//============================================================================
//	DerivedBuffer class
//============================================================================

class DissolveForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DissolveForGPU() = default;
	~DissolveForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	float threshold;
	float edgeSize;
	float padding1[2];
	Vector3 edgeColor;
	float padding2;
	Vector3 thresholdColor;
	float padding3;
};

class GaussianFilterForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GaussianFilterForGPU() = default;
	~GaussianFilterForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	float sigma;
};

class LuminanceBasedOutlineForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LuminanceBasedOutlineForGPU() = default;
	~LuminanceBasedOutlineForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	float strength;
};

class DepthBasedOutlineForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DepthBasedOutlineForGPU() = default;
	~DepthBasedOutlineForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	Matrix4x4 projectionInverse;
};

class VignetteForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	VignetteForGPU() = default;
	~VignetteForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	float scale;
	float power;
};

class RadialBlurForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RadialBlurForGPU() = default;
	~RadialBlurForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	Vector2 center;
	int numSamples;
	float width;
};

class HorizonBlurForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	HorizonBlurForGPU() = default;
	~HorizonBlurForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	int radius;
	float sigma;
};

class VerticalBlurForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	VerticalBlurForGPU() = default;
	~VerticalBlurForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	int radius;
	float sigma;
};

class LuminanceExtractForGPU :
	public IPostProcessBufferSize {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LuminanceExtractForGPU() = default;
	~LuminanceExtractForGPU() = default;

	void ImGui() override;

	//--------- properties ---------------------------------------------------

	float threshold;
};