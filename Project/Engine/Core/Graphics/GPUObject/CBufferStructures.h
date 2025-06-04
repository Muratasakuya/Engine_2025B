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
		const Quaternion& rotation, const Vector3& translation,
		const std::optional<Matrix4x4>& billboardMatrix = std::nullopt);
};

struct Material {

	Color color;
	uint32_t textureIndex;
	uint32_t normalMapTextureIndex;
	int32_t enableNormalMap;
	int32_t enableLighting;
	int32_t enableHalfLambert;
	int32_t enableBlinnPhongReflection;
	int32_t enableImageBasedLighting;
	float environmentCoefficient;
	float shadowRate;
	float phongRefShininess;
	Vector3 specularColor;
	float emissiveIntensity;
	Vector3 emissionColor;
	Matrix4x4 uvTransform;

	void Init();
};

struct SpriteMaterial {

	Matrix4x4 uvTransform;
	Color color;
	Vector3 emissionColor;
	uint32_t useVertexColor;
	float emissiveIntensity;

	void Init();
	void ImGui();
};