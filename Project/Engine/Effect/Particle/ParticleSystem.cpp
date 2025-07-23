#include "ParticleSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Lib/Adapter/EnumAdapter.h>

// imgui
#include <imgui.h>

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

void ParticleSystem::Update() {

	// 所持しているパーティクルの更新
	for (auto& group : gpuParticleGroups_) {

		group.Update();
	}
}

void ParticleSystem::CreateGPUParticle() {

	// 追加して作成
	GPUParticleGroup& group = gpuParticleGroups_.emplace_back();
	group.Create(device_, primitiveType_);
}

void ParticleSystem::CreateCPUParticle() {

	// 追加して作成
	//CPUParticleGroup& group = cpuParticleGroups_.emplace_back();
}

void ParticleSystem::AddParticle() {

	// タイプ選択
	EnumAdapter<ParticleType>::Combo("particleType", &particleType_);
	EnumAdapter<ParticlePrimitiveType>::Combo("primitiveType", &primitiveType_);

	if (ImGui::Button("Add Particle")) {

		// タイプ別で作成
		if (particleType_ == ParticleType::GPU) {

			CreateGPUParticle();
		} else if (particleType_ == ParticleType::CPU) {

			CreateCPUParticle();
		}
	}
}

void ParticleSystem::SelectParticle() {


}

void ParticleSystem::EditParticle() {


}

void ParticleSystem::ImGui() {

	// 追加処理
	AddParticle();
	// 選択処理
	SelectParticle();
	// 値の操作
	EditParticle();
}