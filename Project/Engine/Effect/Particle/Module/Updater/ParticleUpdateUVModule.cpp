#include "ParticleUpdateUVModule.h"

//============================================================================
//	ParticleUpdateUVModule classMethods
//============================================================================

void ParticleUpdateUVModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// UV座標
	Vector3 translation = Vector3::Lerp(particle.uvTranslation.start,
		particle.uvTranslation.target, EasedValue(easing_, particle.progress));

	// uvMatrixの更新
	particle.material.uvTransform = Matrix4x4::MakeAffineMatrix(
		Vector3::AnyInit(1.0f), Vector3::AnyInit(0.0f), translation);
}

void ParticleUpdateUVModule::ImGui() {

	Easing::SelectEasingType(easing_, GetName());
}