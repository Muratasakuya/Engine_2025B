#include "CBufferStructures.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/TransformComponent.h>

// imgui
#include <imgui.h>

//============================================================================
//	CBufferStructures
//============================================================================

void TransformationMatrix::Update(const Transform3DComponent* parent, const Vector3& scale,
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

void Material::ImGui(float itemSize) {

	// 色
	ImGui::SeparatorText("Color");

	ImGui::ColorEdit4("color", &color.r);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
		color.r, color.g,
		color.b, color.a);

	// 発行色
	ImGui::ColorEdit3("emissionColor", &emissionColor.x);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
		emissionColor.x, emissionColor.y,
		emissionColor.z);
	ImGui::PushItemWidth(itemSize);
	ImGui::DragFloat("emissiveIntensity", &emissiveIntensity, 0.01f);
	ImGui::PopItemWidth();

	// Lighting
	ImGui::SeparatorText("Lighting");

	ImGui::PushItemWidth(itemSize);
	ImGui::SliderInt("enableLighting", &enableLighting, 0, 1);
	if (ImGui::SliderInt("phongReflection", &enablePhongReflection, 0, 1)) {
		if (enablePhongReflection) {

			enableBlinnPhongReflection = 0;
		}
	}
	if (ImGui::SliderInt("blinnPhongReflection", &enableBlinnPhongReflection, 0, 1)) {
		if (enableBlinnPhongReflection) {

			enablePhongReflection = 0;
		}
	}
	ImGui::PopItemWidth();

	if (enablePhongReflection ||
		enableBlinnPhongReflection) {

		ImGui::ColorEdit3("specularColor", &specularColor.x);

		ImGui::PushItemWidth(itemSize);
		ImGui::DragFloat("phongRefShininess", &phongRefShininess, 0.01f);
		ImGui::PopItemWidth();
	}

	ImGui::PushItemWidth(168.0f);
	ImGui::DragFloat("shadowRate", &shadowRate, 0.01f, 0.0f, 8.0f);
	ImGui::PopItemWidth();
}