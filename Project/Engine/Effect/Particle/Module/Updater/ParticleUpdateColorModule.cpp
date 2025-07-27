#include "ParticleUpdateColorModule.h"

//============================================================================
//	ParticleUpdateColorModule classMethods
//============================================================================

void ParticleUpdateColorModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 色を補間
	particle.material.color = Color::Lerp(particle.color.start, particle.color.target,
		EasedValue(easing, particle.progress));
}

void ParticleUpdateColorModule::ImGui() {

	Easing::SelectEasingType(easing, "updateColor");
}