#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	ParticleShape
//============================================================================

// 形状
enum class ParticlePrimitiveType {

	Plane,
	Ring,
	Cylinder,
	Count
};

// 平面
struct PlaneForGPU {

	Vector2 size;
	Vector2 pivot;

	void Init() {

		size = Vector2::AnyInit(1.0f);
		pivot = Vector2::AnyInit(0.0f);
	}
};
// リング
struct RingForGPU {

	float outerRadius;
	float innerRadius;
	int divide;

	void Init() {

		outerRadius = 4.0f;
		innerRadius = 2.0f;
		divide = 8;
	}
};
// 円柱
struct CylinderForGPU {

	float topRadius;
	float bottomRadius;
	float height;
	int divide;

	void Init() {

		topRadius = 4.0f;
		bottomRadius = 4.0f;
		height = 4.0f;
		divide = 16;
	}
};