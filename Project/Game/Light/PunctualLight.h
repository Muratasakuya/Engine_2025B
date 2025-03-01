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

	void Init() {

		color = Color::White();
		direction = { 0.0f,-1.0f,0.0f };
		intensity = 0.8f;
	};
};

struct PointLight {

	Color color;
	Vector3 pos;
	float intensity;
	float radius;
	float decay;
	float padding[2];

	void Init() {

		color = Color::White();
		pos = { 0.0f,0.0f,0.0f };
		radius = 5.0f;
		intensity = 0.8f;
		decay = 1.0f;
	};
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

	void Init() {

		color = Color::White();
		pos = { 0.0f,0.0f,0.0f };
		distance = 10.0f;
		intensity = 0.8f;
		direction = { -1.0f,-1.0f,0.0f };
		decay = 1.0f;
		cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
		cosFalloffStart = 1.0f;
	};
};

struct PunctualLight {

	DirectionalLight directional;
	PointLight point;
	SpotLight spot;

	void Init() {

		directional.Init();
		point.Init();
		spot.Init();
	};

};