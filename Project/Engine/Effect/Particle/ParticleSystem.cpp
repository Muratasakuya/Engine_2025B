#include "ParticleSystem.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	ParticleSystem classMethods
//============================================================================

void ParticleSystem::Init(ID3D12Device* device,
	Asset* asset, const std::string& name) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	name_ = name;
}