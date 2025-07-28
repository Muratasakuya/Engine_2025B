#include "ParticleUpdateScaleModule.h"

//============================================================================
//	ParticleUpdateScaleModule classMethods
//============================================================================

void ParticleUpdateScaleModule::Init() {

	// 初期化値
	scale_.start = Vector3::AnyInit(1.0f);
	scale_.target = Vector3::AnyInit(0.0f);
}

void ParticleUpdateScaleModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 色を補間
	particle.transform.scale = Vector3::Lerp(scale_.start, scale_.target,
		EasedValue(easing, particle.progress));
}

void ParticleUpdateScaleModule::ImGui() {

	ImGui::DragFloat3("startScale", &scale_.start.x, 0.01f);
	ImGui::DragFloat3("targetScale", &scale_.target.x, 0.01f);

	Easing::SelectEasingType(easing, GetName());
}