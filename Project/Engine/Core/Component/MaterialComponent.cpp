#include "MaterialComponent.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	MaterialComponent classMethods
//============================================================================

void MaterialComponent::Init() {

	material.Init();

	uvTransform.scale = Vector3::AnyInit(1.0f);
}

void MaterialComponent::UpdateUVTransform() {

	// uvの更新
	material.uvTransform = Matrix4x4::MakeAffineMatrix(
		uvTransform.scale, uvTransform.rotate, uvTransform.translate);
}

void MaterialComponent::ImGui(float itemSize) {

	// 色
	ImGui::SeparatorText("Color");

	ImGui::ColorEdit4("color", &material.color.r);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
		material.color.r, material.color.g,
		material.color.b, material.color.a);

	// 発行色
	ImGui::ColorEdit3("emissionColor", &material.emissionColor.x);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
		material.emissionColor.x, material.emissionColor.y,
		material.emissionColor.z);
	ImGui::PushItemWidth(itemSize);
	ImGui::DragFloat("emissiveIntensity", &material.emissiveIntensity, 0.01f);
	ImGui::PopItemWidth();

	// UV
	ImGui::SeparatorText("UV");

	// transform
	ImGui::PushItemWidth(itemSize);
	ImGui::DragFloat2("uvTranslate", &uvTransform.translate.x, 0.1f);
	ImGui::SliderAngle("uvRotate", &uvTransform.rotate.z);
	ImGui::DragFloat2("uvScale", &uvTransform.scale.x, 0.1f);
	ImGui::PopItemWidth();

	// Lighting
	ImGui::SeparatorText("Lighting");

	ImGui::PushItemWidth(itemSize);
	ImGui::SliderInt("enableLighting", &material.enableLighting, 0, 1);
	if (ImGui::SliderInt("phongReflection", &material.enablePhongReflection, 0, 1)) {
		if (material.enablePhongReflection) {

			material.enableBlinnPhongReflection = 0;
		}
	}
	if (ImGui::SliderInt("blinnPhongReflection", &material.enableBlinnPhongReflection, 0, 1)) {
		if (material.enableBlinnPhongReflection) {

			material.enablePhongReflection = 0;
		}
	}
	ImGui::PopItemWidth();

	if (material.enablePhongReflection ||
		material.enableBlinnPhongReflection) {

		ImGui::ColorEdit3("specularColor", &material.specularColor.x);

		ImGui::PushItemWidth(itemSize);
		ImGui::DragFloat("phongRefShininess", &material.phongRefShininess, 0.01f);
		ImGui::PopItemWidth();
	}

	ImGui::PushItemWidth(168.0f);
	ImGui::DragFloat("shadowRate", &material.shadowRate, 0.01f, 0.0f, 8.0f);
	ImGui::PopItemWidth();
}