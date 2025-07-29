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

	Plane,    // 平面
	Ring,     // リング
	Cylinder, // 円柱
	Crescent, // 三日月
	Count
};

// 平面
struct PlaneForGPU {

	Vector2 size;
	Vector2 pivot;

	void Init() {

		size = Vector2::AnyInit(1.0f);
		pivot = Vector2::AnyInit(0.5f);
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
struct CrescentForGPU {

	float outerRadius;
	float innerRadius;

	float startAngle;
	float endAngle;

	float lattice;
	Vector2 pivot;
	int divide;

	// 0: 縦
	// 1: 横
	int uvMode;

	void Init() {

		outerRadius = 4.0f;
		innerRadius = 2.0f;

		// 30度~150度
		startAngle = pi / 6.0f;
		endAngle = pi * 5.0f / 6.0f;

		lattice = 0.5f;
		pivot = Vector2::AnyInit(0.5f);
		divide = 8;
		uvMode = 1;
	}
};