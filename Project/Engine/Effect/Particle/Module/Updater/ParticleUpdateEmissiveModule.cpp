#include "ParticleUpdateEmissiveModule.h"

//============================================================================
//	ParticleUpdateEmissiveModule classMethods
//============================================================================

void ParticleUpdateEmissiveModule::Init() {

	// 初期値の設定
	intencity_.start = 1.0f;
	intencity_.target = 1.0f;

	color_.start = Vector3::AnyInit(1.0f);
	color_.target = Vector3::AnyInit(1.0f);
}

void ParticleUpdateEmissiveModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 発光度
	particle.material.emissiveIntecity = std::lerp(
		intencity_.start, intencity_.target, EasedValue(easingType_, particle.progress));
	// 色
	particle.material.emissionColor = Vector3::Lerp(
		color_.start, color_.target, EasedValue(easingType_, particle.progress));
}

void ParticleUpdateEmissiveModule::ImGui() {

	ImGui::DragFloat("startIntencity", &intencity_.start, 0.01f);
	ImGui::DragFloat("targetIntencity", &intencity_.target, 0.01f);

	ImGui::ColorEdit3("startColor", &color_.start.x);
	ImGui::ColorEdit3("targetColor", &color_.target.x);

	Easing::SelectEasingType(easingType_, GetName());
}