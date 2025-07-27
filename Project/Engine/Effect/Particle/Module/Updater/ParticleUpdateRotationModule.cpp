#include "ParticleUpdateRotationModule.h"

//============================================================================
//	ParticleUpdateRotationModule classMethods
//============================================================================

void ParticleUpdateRotationModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	ParticleBillboardType type = static_cast<ParticleBillboardType>(particle.transform.billboardMode);
	switch (type) {
	case ParticleBillboardType::None:
	case ParticleBillboardType::YAxis:

		particle.transform.rotationMatrix =
			Matrix4x4::MakeRotateMatrix(particle.rotation);
		break;
	case ParticleBillboardType::All:

		particle.transform.rotationMatrix = Matrix4x4::MakeIdentity4x4();
		break;
	}
}

void ParticleUpdateRotationModule::ImGui() {

	Easing::SelectEasingType(easing, "updateRotation");
}