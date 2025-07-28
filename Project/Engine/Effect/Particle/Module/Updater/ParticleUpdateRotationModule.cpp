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

		// 更新処理分岐
		switch (updateType_) {
		case ParticleUpdateRotationModule::UpdateType::Lerp:

			// 回転補間
			rotation = Vector3::Lerp(lerpRotation_.start,
				lerpRotation_.target, EasedValue(easing_, particle.progress));
			break;
		case ParticleUpdateRotationModule::UpdateType::ConstantAdd:

			// 回転加算
			rotation = particle.transform.rotationMatrix.GetRotationValue();
			rotation += addRotation_;
			break;
		}

		// 回転行列の更新
		particle.transform.rotationMatrix = Matrix4x4::MakeRotateMatrix(rotation);
		break;
	case ParticleBillboardType::All:

		particle.transform.rotationMatrix = Matrix4x4::MakeIdentity4x4();
		break;
	}
}

void ParticleUpdateRotationModule::ImGui() {

	EnumAdapter<ParticleBillboardType>::Combo("billboardType", &billboardType_);
	EnumAdapter<UpdateType>::Combo("updateType", &updateType_);

	switch (updateType_) {
	case ParticleUpdateRotationModule::UpdateType::Lerp:

		ImGui::DragFloat3("startRotation", &lerpRotation_.start.x, 0.01f);
		ImGui::DragFloat3("targetRotation", &lerpRotation_.target.x, 0.01f);

		Easing::SelectEasingType(easing_, GetName());
		break;
	case ParticleUpdateRotationModule::UpdateType::ConstantAdd:

		ImGui::DragFloat3("addRotation", &addRotation_.x, 0.01f);
		break;
	}
}