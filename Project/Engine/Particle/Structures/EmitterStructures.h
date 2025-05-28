#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Quaternion.h>

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

	float radius = 0.2f;
	Vector3 center = Vector3::AnyInit(0.0f);
};
// 半球
struct EmitterHemisphere {

	float radius = 0.2f;
	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
	Quaternion rotation = Quaternion::IdentityQuaternion();
};
// 箱(OBB)
struct EmitterBox {

	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 size = Vector3::AnyInit(1.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
	Quaternion rotation = Quaternion::IdentityQuaternion();
};
// コーン状
struct EmitterCone {

	float baseRadius = 0.4f;
	float topRadius = 0.8f;
	float height = 1.0f;
	Vector3 center = Vector3::AnyInit(0.0f);
	Vector3 eulerRotate = Vector3::AnyInit(0.0f);
	Quaternion rotation = Quaternion::IdentityQuaternion();
};
namespace EmitterShape {

	// imgui選択
	void SelectEmitterShape(EmitterShapeType& emitterShape, const std::string& label = "label");

	// 値操作
	void EditEmitterShape(const EmitterShapeType& emitterShape,
		EmitterSphere& sphere, EmitterHemisphere& hemisphere,
		EmitterBox& box, EmitterCone& cone);

	// 適応
	void Aplly(const Json& data, EmitterSphere& sphere, EmitterHemisphere& hemisphere,
		EmitterBox& box, EmitterCone& cone);

	// 保存
	void Save(Json& data, const EmitterSphere& sphere, const EmitterHemisphere& hemisphere,
		const EmitterBox& box, const EmitterCone& cone);
}