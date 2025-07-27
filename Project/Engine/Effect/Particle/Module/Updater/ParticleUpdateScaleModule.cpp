#include "ParticleUpdateScaleModule.h"

//============================================================================
//	ParticleUpdateScaleModule classMethods
//============================================================================

void ParticleUpdateScaleModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 色を補間
	particle.transform.scale = Vector3::Lerp(particle.scale.start, particle.scale.target,
		EasedValue(easing, particle.progress));
}

void ParticleUpdateScaleModule::ImGui() {

	Easing::SelectEasingType(easing, "updateScale");
}