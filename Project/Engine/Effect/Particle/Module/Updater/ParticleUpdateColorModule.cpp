#include "ParticleUpdateColorModule.h"

//============================================================================
//	ParticleUpdateColorModule classMethods
//============================================================================

void ParticleUpdateColorModule::Init() {

	// 初期化値
	color_.start = Color::White();
	color_.target = Color::White(0.0f);
}

void ParticleUpdateColorModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 色を補間
	particle.material.color = Color::Lerp(color_.start, color_.target,
		EasedValue(easing, particle.progress));
}

void ParticleUpdateColorModule::ImGui() {

	ImGui::ColorEdit4("startColor", &color_.start.r);
	ImGui::ColorEdit4("targetColor", &color_.target.r);

	Easing::SelectEasingType(easing, GetName());
}