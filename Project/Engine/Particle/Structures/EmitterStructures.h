#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector3.h>

//c++
#include <string>
#include <variant>

//============================================================================
//	Emitter structures
//============================================================================

// emitterの形状
enum class EmitterShapeType {

	Sphere,
	Hemisphere,
	Box,
	Cone
};
// 各emitterごとの必要な値
// defaultの値を設定
// 球
struct EmitterSphere {

	float radius = 1.0f;
	Vector3 center = Vector3::AnyInit(0.0f);
};
// 半球
struct EmitterHemisphere {

	float radius = 1.0f;
	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
};
// 箱(OBB)
struct EmitterBox {

	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 size = Vector3::AnyInit(1.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
};
// コーン状
struct EmitterCone {

	float baseRadius = 1.0f;
	float topRadius = 1.6f;
	float height = 1.6f;
	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
};
namespace EmitterShape {

	// imgui選択
	void SelectEmitterShape(EmitterShapeType& emitterShape, const std::string& label = "label");

	// 値操作
	void EditEmitterShape(const EmitterShapeType& emitterShape,
		EmitterSphere& sphere, EmitterHemisphere& hemisphere,
		EmitterBox& box, EmitterCone& cone);
}