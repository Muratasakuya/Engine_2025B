#include "ParticleUpdatePrimitiveModule.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	ParticleUpdatePrimitiveModule classMethods
//============================================================================

void ParticleUpdatePrimitiveModule::Init() {

	// 初期化値、全ての形状を初期化
	primitive_.start.plane.Init();
	primitive_.target.plane.Init();

	primitive_.start.ring.Init();
	primitive_.target.ring.Init();

	primitive_.start.cylinder.Init();
	primitive_.target.cylinder.Init();
}

void ParticleUpdatePrimitiveModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 形状別で補間
	type_ = particle.primitive.type;
	switch (type_) {
	case ParticlePrimitiveType::Plane: {

		UpdatePlane(particle);
		break;
	}
	case ParticlePrimitiveType::Ring: {

		UpdateRing(particle);
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		UpdateCylinder(particle);
		break;
	}
	}
}

//============================================================================
//	Plane
//============================================================================
void ParticleUpdatePrimitiveModule::UpdatePlane(CPUParticle::ParticleData& particle) {

	particle.primitive.plane.size = Vector2::Lerp(primitive_.start.plane.size,
		primitive_.target.plane.size, EasedValue(easingType_, particle.progress));
}

//============================================================================
//	Ring
//============================================================================
void ParticleUpdatePrimitiveModule::UpdateRing(CPUParticle::ParticleData& particle) {

	particle.primitive.ring.divide = Algorithm::LerpInt(primitive_.start.ring.divide,
		primitive_.target.ring.divide, EasedValue(easingType_, particle.progress));

	particle.primitive.ring.outerRadius = std::lerp(primitive_.start.ring.outerRadius,
		primitive_.target.ring.outerRadius, EasedValue(easingType_, particle.progress));

	particle.primitive.ring.innerRadius = std::lerp(primitive_.start.ring.innerRadius,
		primitive_.target.ring.innerRadius, EasedValue(easingType_, particle.progress));
}

//============================================================================
//	Cylinder
//============================================================================
void ParticleUpdatePrimitiveModule::UpdateCylinder(CPUParticle::ParticleData& particle) {

	particle.primitive.cylinder.divide = Algorithm::LerpInt(primitive_.start.cylinder.divide,
		primitive_.target.cylinder.divide, EasedValue(easingType_, particle.progress));

	particle.primitive.cylinder.topRadius = std::lerp(primitive_.start.cylinder.topRadius,
		primitive_.target.cylinder.topRadius, EasedValue(easingType_, particle.progress));

	particle.primitive.cylinder.bottomRadius = std::lerp(primitive_.start.cylinder.bottomRadius,
		primitive_.target.cylinder.bottomRadius, EasedValue(easingType_, particle.progress));

	particle.primitive.cylinder.height = std::lerp(primitive_.start.cylinder.height,
		primitive_.target.cylinder.height, EasedValue(easingType_, particle.progress));
}

void ParticleUpdatePrimitiveModule::ImGui() {

	Easing::SelectEasingType(easingType_, GetName());

	switch (type_) {
	case ParticlePrimitiveType::Plane: {

		ImGui::DragFloat2("startSize", &primitive_.start.plane.size.x, 0.01f);
		ImGui::DragFloat2("targetSize", &primitive_.target.plane.size.x, 0.01f);
		break;
	}
	case ParticlePrimitiveType::Ring: {

		ImGui::DragInt("startDivide", &primitive_.start.ring.divide, 1, 3, 32);
		ImGui::DragInt("targetDivide", &primitive_.target.ring.divide, 1, 3, 32);

		ImGui::DragFloat("startOuterRadius", &primitive_.start.ring.outerRadius, 0.01f);
		ImGui::DragFloat("targetOuterRadius", &primitive_.target.ring.outerRadius, 0.01f);

		ImGui::DragFloat("startInnerRadius", &primitive_.start.ring.innerRadius, 0.01f);
		ImGui::DragFloat("targetInnerRadius", &primitive_.target.ring.innerRadius, 0.01f);
		break;
	}
	case ParticlePrimitiveType::Cylinder: {

		ImGui::DragInt("startDivide", &primitive_.start.cylinder.divide, 1, 3, 32);
		ImGui::DragInt("targetDivide", &primitive_.target.cylinder.divide, 1, 3, 32);

		ImGui::DragFloat("startTopRadius", &primitive_.start.cylinder.topRadius, 0.01f);
		ImGui::DragFloat("targetTopRadius", &primitive_.target.cylinder.topRadius, 0.01f);

		ImGui::DragFloat("startTopBottomRadius", &primitive_.start.cylinder.bottomRadius, 0.01f);
		ImGui::DragFloat("targetBottomRadius", &primitive_.target.cylinder.bottomRadius, 0.01f);

		ImGui::DragFloat("startHeight", &primitive_.start.cylinder.height, 0.01f);
		ImGui::DragFloat("targetHeight", &primitive_.target.cylinder.height, 0.01f);
		break;
	}
	}
}