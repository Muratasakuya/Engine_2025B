#include "ParticleSystem.h"

//============================================================================
//	ParticleSystem classMethods
//============================================================================

ParticleSystem* ParticleSystem::instance_ = nullptr;

ParticleSystem* ParticleSystem::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ParticleSystem();
	}
	return instance_;
}

void ParticleSystem::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ParticleSystem::Init(CameraManager* cameraManager) {

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;
}

void ParticleSystem::Update() {


}