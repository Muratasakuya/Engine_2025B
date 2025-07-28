#include "ParticleUpdateUVModule.h"

//============================================================================
//	ParticleUpdateUVModule classMethods
//============================================================================

void ParticleUpdateUVModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// UV座標
	Vector3 translation = Vector3::Lerp(translation_.start,
		translation_.target, EasedValue(easing_, particle.progress));

	// uvMatrixの更新
	particle.material.uvTransform = Matrix4x4::MakeAffineMatrix(
		Vector3::AnyInit(1.0f), Vector3::AnyInit(0.0f), translation);
}

void ParticleUpdateUVModule::ImGui() {

	ImGui::DragFloat2("startTranslation", &translation_.start.x, 0.01f);
	ImGui::DragFloat2("targetTranslation", &translation_.target.x, 0.01f);

	Easing::SelectEasingType(easing_, GetName());
}