#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Vector4.h>
#include <Lib/MathUtils/Quaternion.h>
#include <Lib/MathUtils/Matrix4x4.h>

// c++
#include <cstdint>
#include <string>
// front
class BaseTransform;

//============================================================================
//	CBufferStructures
//============================================================================

struct TransformationMatrix {

	Matrix4x4 world;
	Matrix4x4 worldInverseTranspose;

	void Update(const BaseTransform* parent, const Vector3& scale,
		const Quaternion& rotation, const Vector3& translation);
};

struct MSTestTransformationMatrix {

	Matrix4x4 world;
	Matrix4x4 viewProjection;
};

struct Material {

	Color color;
	uint32_t textureIndex;
	int32_t enableLighting;
	int32_t enableHalfLambert;
	int32_t enablePhongReflection;
	int32_t enableBlinnPhongReflection;
	float shadowRate;
	float phongRefShininess;
	Vector3 specularColor;
	float emissiveIntensity;
	Vector3 emissionColor;
	Matrix4x4 uvTransform;

	void Init();
};

struct EffectMaterial {

	Color color;
	uint32_t textureIndex;
	float emissiveIntensity;
	Vector3 emissionColor;
	Matrix4x4 uvTransform;

	void Init();
};

struct SpriteMaterial {

	Color color;
	Matrix4x4 uvTransform;

	void Init();
	void ImGui();
};