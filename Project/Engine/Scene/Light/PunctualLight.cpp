#include "PunctualLight.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	Light Methods
//============================================================================

void DirectionalLight::Init() {

	color = Color::White();
	direction = { 0.0f,-1.0f,0.0f };
	intensity = 1.4f;
}

void DirectionalLight::ImGui(float itemWidth) {

	ImGui::SeparatorText("DirectionalLight");

	ImGui::PushItemWidth(itemWidth);

	ImGui::ColorEdit4("color##Directional", &color.r);
	ImGui::DragFloat3("direction##Directional", &direction.x, 0.1f);
	ImGui::DragFloat("intensity##Directional", &intensity, 0.01f);

	ImGui::PopItemWidth();
}

void PointLight::Init() {

	color = Color::White();
	pos = { 0.0f,8.0f,0.0f };
	radius = 5.0f;
	intensity = 0.8f;
	decay = 1.0f;
}

void PointLight::ImGui(float itemWidth) {

	ImGui::SeparatorText("PointLight");

	ImGui::PushItemWidth(itemWidth);

	ImGui::ColorEdit4("color##PointLight", &color.r);
	ImGui::DragFloat3("pos##PointLight", &pos.x, 0.1f);
	ImGui::DragFloat("radius##PointLight", &radius, 0.01f);
	ImGui::DragFloat("intensity##PointLight", &intensity, 0.01f);
	ImGui::DragFloat("decay##PointLight", &decay, 0.01f);

	ImGui::PopItemWidth();
}

void SpotLight::Init() {

	color = Color::White();
	pos = { 0.0f,8.0f,0.0f };
	distance = 10.0f;
	intensity = 0.8f;
	direction = { 0.0f,-1.0f,0.0f };
	decay = 1.0f;
	cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	cosFalloffStart = 1.0f;
}

void SpotLight::ImGui(float itemWidth) {

	ImGui::SeparatorText("SpotLight");

	ImGui::PushItemWidth(itemWidth);

	ImGui::ColorEdit4("color##SpotLight", &color.r);
	ImGui::DragFloat3("pos##SpotLight", &pos.x, 0.1f);
	ImGui::DragFloat3("direction##SpotLight", &direction.x, 0.01f);
	ImGui::DragFloat("distance##SpotLight", &distance, 0.01f);
	ImGui::DragFloat("intensity##SpotLight", &intensity, 0.01f);
	ImGui::DragFloat("decay##SpotLight", &decay, 0.01f);
	ImGui::DragFloat("cosAngle##SpotLight", &cosAngle, 0.01f);
	ImGui::DragFloat("cosFalloffStart##SpotLight", &cosFalloffStart, 0.01f);

	ImGui::PopItemWidth();
}

//============================================================================
//	PunctualLight classMethods
//============================================================================

void PunctualLight::Init() {

	directional.Init();
	point.Init();
	spot.Init();

	preDirectionalLightDirection_ = directional.direction;
	preSpotLightDirection_ = spot.direction;
}

void PunctualLight::Update() {

	// 向きに変更があった場合のみ正規化
	if (preDirectionalLightDirection_ != directional.direction) {
		
		directional.direction = directional.direction.Normalize();
		preDirectionalLightDirection_ = directional.direction;
	}

	if (preSpotLightDirection_ != spot.direction) {

		directional.direction = spot.direction.Normalize();
		preSpotLightDirection_ = spot.direction;
	}
}

void PunctualLight::ImGui() {

	// 各lightのImGui表示
	directional.ImGui(itemWidth_);
	point.ImGui(itemWidth_);
	spot.ImGui(itemWidth_);
}