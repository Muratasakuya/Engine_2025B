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
};
// リング
struct RingForGPU {

	float outerRadius;
	float innerRadius;
	int divide;
};
// 円柱
struct CylinderForGPU {

	float topRadius;
	float bottomRadius;
	float height;
	int divide;
};