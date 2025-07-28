#include "ParticleUpdateRotationModule.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ParticleUpdateRotationModule classMethods
//============================================================================

void ParticleUpdateRotationModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	particle.transform.billboardMode = static_cast<uint32_t>(billboardType_);
	Vector3 rotation{};
	switch (billboardType_) {
	case ParticleBillboardType::None:
	case ParticleBillboardType::YAxis:

		rotation = Vector3::Lerp(rotation_.start,
			rotation_.target, EasedValue(easing, particle.progress));

		particle.transform.rotationMatrix = Matrix4x4::MakeRotateMatrix(rotation);
		break;
	case ParticleBillboardType::All:

		particle.transform.rotationMatrix = Matrix4x4::MakeIdentity4x4();
		break;
	}
}

void ParticleUpdateRotationModule::ImGui() {

	EnumAdapter<ParticleBillboardType>::Combo("billboardType", &billboardType_);

	ImGui::DragFloat3("startRotation", &rotation_.start.x, 0.01f);
	ImGui::DragFloat3("targetRotation", &rotation_.target.x, 0.01f);

	Easing::SelectEasingType(easing, GetName());
}