#include "MaterialComponent.h"

//============================================================================
//	include
//============================================================================
#include <Game/Time/GameTimer.h>
#include <Lib/Adapter/JsonAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	MaterialComponent classMethods
//============================================================================

void MaterialComponent::Init() {

	material.Init();
	uvTransform.scale = Vector3::AnyInit(1.0f);
	prevUVTransform.scale = Vector3::AnyInit(1.0f);
}

void MaterialComponent::UpdateUVTransform() {

	// 値に変更がなければ更新しない
	if (uvTransform == prevUVTransform) {
		return;
	}

	// uvの更新
	material.uvTransform = Matrix4x4::MakeAffineMatrix(
		uvTransform.scale, uvTransform.rotate, uvTransform.translate);

	// 値を保存
	prevUVTransform = uvTransform;
}

void MaterialComponent::ImGui(float itemSize) {

	// 色
	ImGui::SeparatorText("Color");

	ImGui::PushItemWidth(itemSize);
	ImGui::ColorEdit4("color", &material.color.r);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
		material.color.r, material.color.g,
		material.color.b, material.color.a);

	// 発行色
	ImGui::ColorEdit3("emissionColor", &material.emissionColor.x);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
		material.emissionColor.x, material.emissionColor.y,
		material.emissionColor.z);
	ImGui::DragFloat("emissiveIntensity", &material.emissiveIntensity, 0.01f);

	// UV
	ImGui::SeparatorText("UV");

	// transform
	ImGui::DragFloat2("uvTranslate", &uvTransform.translate.x, 0.1f);
	ImGui::SliderAngle("uvRotate", &uvTransform.rotate.z);
	ImGui::DragFloat2("uvScale", &uvTransform.scale.x, 0.1f);

	// Lighting
	ImGui::SeparatorText("Lighting");

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

	if (material.enablePhongReflection ||
		material.enableBlinnPhongReflection) {

		ImGui::ColorEdit3("specularColor", &material.specularColor.x);

		ImGui::DragFloat("phongRefShininess", &material.phongRefShininess, 0.01f);
	}

	ImGui::DragFloat("shadowRate", &material.shadowRate, 0.01f, 0.0f, 8.0f);
	ImGui::PopItemWidth();
}

void MaterialComponent::ToJson(Json& data) {

	// Material
	// color
	data["color"] = material.color.ToJson();
	data["emissionColor"] = material.emissionColor.ToJson();
	data["emissiveIntensity"] = material.emissiveIntensity;
	// lighting
	data["enableLighting"] = material.enableLighting;
	data["enableHalfLambert"] = material.enableHalfLambert;
	data["enablePhongReflection"] = material.enablePhongReflection;
	data["enableBlinnPhongReflection"] = material.enableBlinnPhongReflection;
	data["phongRefShininess"] = material.phongRefShininess;
	data["specularColor"] = material.specularColor.ToJson();
	data["shadowRate"] = material.shadowRate;

	// UV
	data["uvScale"] = uvTransform.scale.ToJson();
	data["uvRotate"] = uvTransform.rotate.ToJson();
	data["uvTranslate"] = uvTransform.translate.ToJson();
}

void MaterialComponent::FromJson(const Json& data) {

	// Material
	// color
	material.color = JsonAdapter::ToObject<Color>(data["color"]);
	material.emissionColor = JsonAdapter::ToObject<Vector3>(data["emissionColor"]);
	material.emissiveIntensity = data["emissiveIntensity"];
	// lighting
	material.enableLighting = data["enableLighting"];
	material.enableHalfLambert = data["enableHalfLambert"];
	material.enablePhongReflection = data["enablePhongReflection"];
	material.enableBlinnPhongReflection = data["enableBlinnPhongReflection"];
	material.phongRefShininess = data["phongRefShininess"];
	material.specularColor = JsonAdapter::ToObject<Vector3>(data["specularColor"]);
	material.shadowRate = data["shadowRate"];

	// UV
	uvTransform.scale = JsonAdapter::ToObject<Vector3>(data["uvScale"]);
	uvTransform.rotate = JsonAdapter::ToObject<Vector3>(data["uvRotate"]);
	uvTransform.translate = JsonAdapter::ToObject<Vector3>(data["uvTranslate"]);
}

//============================================================================
//	EffectMaterialComponent classMethods
//============================================================================

void EffectMaterialComponent::Init() {

	material.Init();
	uvTransform.scale = Vector3::AnyInit(1.0f);
}

void EffectMaterialComponent::UpdateUVTransform() {

	// uvScrollを更新
	UpdateUVScroll();

	// 値に変更がなければ更新しない
	if (uvTransform == prevUVTransform_) {
		return;
	}

	// uvの更新
	material.uvTransform = Matrix4x4::MakeAffineMatrix(
		uvTransform.scale, uvTransform.rotate, uvTransform.translate);

	// 値を保存
	prevUVTransform_ = uvTransform;
}

void EffectMaterialComponent::UpdateUVScroll() {

	// スクロール量を計算
	uvScrollAnimation_.MoveValue(uvTransform.translate.x);
}

void EffectMaterialComponent::EditMaterial(float itemSize) {

	// 色
	ImGui::SeparatorText("Color");

	ImGui::PushItemWidth(itemSize);
	ImGui::ColorEdit4("color", &material.color.r);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
		material.color.r, material.color.g,
		material.color.b, material.color.a);

	// 頂点カラーを使用するかどうか
	ImGui::SliderInt("useVertexColor", &material.useVertexColor, 0, 1);

	// discard閾値
	ImGui::DragFloat("alphaReference", &material.alphaReference, 0.01f);

	// 発行色
	ImGui::ColorEdit3("emissionColor", &material.emissionColor.x);
	ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
		material.emissionColor.x, material.emissionColor.y,
		material.emissionColor.z);
	ImGui::DragFloat("emissiveIntensity", &material.emissiveIntensity, 0.01f);

	// UV
	ImGui::SeparatorText("UV");

	// transform
	ImGui::DragFloat2("uvTranslate", &uvTransform.translate.x, 0.1f);
	ImGui::SliderAngle("uvRotate", &uvTransform.rotate.z);
	ImGui::DragFloat2("uvScale", &uvTransform.scale.x, 0.1f);

	ImGui::PopItemWidth();
}

void EffectMaterialComponent::ImGui(float itemSize) {

	if (ImGui::BeginTabBar("PrimitiveMeshTabs")) {

		if (ImGui::BeginTabItem("Material")) {

			// material
			EditMaterial(itemSize);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("UVAnimation")) {

			if (ImGui::Button("Start", ImVec2(itemSize, 32.0f))) {

				uvScrollAnimation_.Start();
				uvTransform.translate.x = 0.0f;
			}

			if (ImGui::Button("Reset", ImVec2(itemSize, 32.0f))) {

				uvScrollAnimation_.Reset();
			}

			// uvScroll
			uvScrollAnimation_.ImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void EffectMaterialComponent::ToJson(Json& data) {

	// Material
	// color
	data["color"] = material.color.ToJson();
	data["emissionColor"] = material.emissionColor.ToJson();
	data["emissiveIntensity"] = material.emissiveIntensity;
	data["alphaReference"] = material.alphaReference;

	// UV
	data["uvScale"] = uvTransform.scale.ToJson();
	data["uvRotate"] = uvTransform.rotate.ToJson();
	data["uvTranslate"] = uvTransform.translate.ToJson();
}

void EffectMaterialComponent::FromJson(const Json& data) {

	// Material
	// color
	material.color = JsonAdapter::ToObject<Color>(data["color"]);
	material.emissionColor = JsonAdapter::ToObject<Vector3>(data["emissionColor"]);
	material.emissiveIntensity = data["emissiveIntensity"];
	material.alphaReference = data["alphaReference"];

	// UV
	uvTransform.scale = JsonAdapter::ToObject<Vector3>(data["uvScale"]);
	uvTransform.rotate = JsonAdapter::ToObject<Vector3>(data["uvRotate"]);
	uvTransform.translate = JsonAdapter::ToObject<Vector3>(data["uvTranslate"]);
}

void EffectMaterialComponent::SetUVScrollValue(float value) {

	// スクロール量を設定
	uvScrollAnimation_.move_.moveValue = value;
}

void swap(EffectMaterialComponent& a, EffectMaterialComponent& b) noexcept {

	using std::swap;
	swap(a.material, b.material);
	swap(a.uvTransform, b.uvTransform);
	swap(a.prevUVTransform_, b.prevUVTransform_);
	swap(a.uvScrollAnimation_, b.uvScrollAnimation_);
}