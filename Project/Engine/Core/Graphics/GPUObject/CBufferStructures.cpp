#include "CBufferStructures.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>

// imgui
#include <imgui.h>

//============================================================================
//	CBufferStructures
//============================================================================

void TransformationMatrix::Update(const BaseTransform* parent, const Vector3& scale,
	const Quaternion& rotation, const Vector3& translation) {

	world = Matrix4x4::MakeAxisAffineMatrix(
		scale, rotation, translation);
	if (parent) {
		world = world * parent->matrix.world;
	}
	worldInverseTranspose = Matrix4x4::Transpose(Matrix4x4::Inverse(world));
}

void Material::Init() {

	color = Color::White();
	enableLighting = true;
	enableHalfLambert = true;
	enablePhongReflection = false;
	enableBlinnPhongReflection = false;
	shadowRate = 0.25f;
	phongRefShininess = 1.0f;
	specularColor = Vector3(1.0f, 1.0f, 1.0f);
	emissiveIntensity = 0.0f;
	emissionColor = Vector3(1.0f, 1.0f, 1.0f);
	uvTransform = Matrix4x4::MakeIdentity4x4();
}

void EffectMaterial::Init() {

	color = Color::White();
	emissiveIntensity = 0.0f;
	emissionColor = Vector3(1.0f, 1.0f, 1.0f);
	uvTransform = Matrix4x4::MakeIdentity4x4();
}

void SpriteMaterial::Init() {

	color = Color::White();
	uvTransform = Matrix4x4::MakeIdentity4x4();
}

void SpriteMaterial::ImGui() {

	ImGui::ColorEdit4("color", &color.r);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
		color.r, color.g,
		color.b, color.a);
}