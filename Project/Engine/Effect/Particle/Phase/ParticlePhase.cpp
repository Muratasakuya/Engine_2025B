#include "ParticlePhase.h"

//============================================================================
//	ParticlePhase classMethods
//============================================================================

void ParticlePhase::Emit(std::list<CPUParticle::ParticleData>& particles) {

	// 発生処理
	spawner->Execute(particles);
}

void ParticlePhase::Update(CPUParticle::ParticleData& particle, float deltaTime) {

	// 更新処理
	for (const auto& updater : updaters) {

		updater->Execute(particle, deltaTime);
	}
}

void ParticlePhase::ImGui() {


}