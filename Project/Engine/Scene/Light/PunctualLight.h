#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Vector4.h>

// c++
#include <numbers>

//============================================================================
//	PunctualLight
//============================================================================

struct DirectionalLight {

	Color color;
	Vector3 direction;
	float intensity;

	void Init();

	void ImGui(float itemWidth);
};

struct PointLight {

	Color color;
	Vector3 pos;
	float intensity;
	float radius;
	float decay;
	float padding[2];

	void Init();

	void ImGui(float itemWidth);
};

struct SpotLight {

	Color color;
	Vector3 pos;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	float padding[2];

	void Init();

	void ImGui(float itemWidth);
};

//============================================================================
//	PunctualLight class
//============================================================================
class PunctualLight {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PunctualLight() = default;
	~PunctualLight() = default;

	//--------- functions ----------------------------------------------------

	void Init();

	void ImGui();

	//--------- variables ----------------------------------------------------

	DirectionalLight directional;
	PointLight point;
	SpotLight spot;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemWidth_ = 162.0f;
};