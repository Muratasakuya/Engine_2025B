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

void GameEffect::ResetEmitFlag() {

	// リセット
	emitOnce_ = false;
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

	if (hasParent_) {
		return;
	}

	// 親を設定
	// GPU
	for (auto& group : particleSystem_->GetGPUGroup()) {

		group.group.SetParent(true, parent);
	}
	// CPU
	for (auto& group : particleSystem_->GetCPUGroup()) {

		group.group.SetParent(true, parent);
	}

	// 親を設定した
	hasParent_ = true;
}

void GameEffect::FrequencyEmit() {

	// 一定間隔で発生
	particleSystem_->FrequencyEmit();
}

void GameEffect::Emit(bool emitOnce) {

	// 発生済みならリセットするまで発生できない
	if (emitOnce_) {
		return;
	}

	// 強制的に発生
	particleSystem_->Emit();

	emitOnce_ = emitOnce;
}