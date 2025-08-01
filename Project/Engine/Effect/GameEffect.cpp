#include "GameEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Core/ParticleManager.h>

//============================================================================
//	GameEffect classMethods
//============================================================================

void GameEffect::CreateParticleSystem(const std::string& filePath) {

	// Managerに渡して作成
	particleSystem_ = ParticleManager::GetInstance()->CreateParticleSystem(filePath);
}

void GameEffect::SetTransform(const Matrix4x4& matrix) {

	// 座標を設定
	// GPU
	for (auto& group : particleSystem_->GetGPUGroup()) {

		group.group.SetTransform(matrix);
	}
	// CPU
	for (auto& group : particleSystem_->GetCPUGroup()) {

		group.group.SetTransform(matrix);
	}
}

void GameEffect::SetParent(const BaseTransform& parent) {

	// 親を設定
	// GPU
	for (auto& group : particleSystem_->GetGPUGroup()) {

		group.group.SetParent(true, parent);
	}
	// CPU
	for (auto& group : particleSystem_->GetCPUGroup()) {

		group.group.SetParent(true, parent);
	}
}

void GameEffect::FrequencyEmit() {

	// 一定間隔で発生
	particleSystem_->FrequencyEmit();
}

void GameEffect::Emit() {

	// 強制的に発生
	particleSystem_->Emit();
}