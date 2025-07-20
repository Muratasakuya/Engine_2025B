#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	ParticleShape
//============================================================================

// 平面
struct PlaneForGPU {

	Vector2 size;
};
// リング
struct RingForGPU {

	float outerRadius;
	float innerRadius;
	int divide;
	Matrix4x4 world;
};
// 円柱
struct CylinderForGPU {

	float topRadius;
	float bottomRadius;
	float height;
	int divide;
	Matrix4x4 world;
};

// 形状
enum class ParticleShape {

	Plane,
	Ring,
	Cylinder,
	Count
};