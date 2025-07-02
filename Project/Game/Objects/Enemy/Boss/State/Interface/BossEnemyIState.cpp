#include "BossEnemyIState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Particle/ParticleSystem.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyIState classMethods
//============================================================================

void BossEnemyIState::LookTarget(BossEnemy& bossEnemy, const Vector3& target) {

	const float deltaTime = GameTimer::GetDeltaTime();

	// 前方ベクトルを取得
	Vector3 bossPos = bossEnemy.GetTranslation();

	// 回転を計算して設定
	Quaternion bossRotation = Quaternion::LookTarget(bossPos, target,
		Vector3(0.0f, 1.0f, 0.0f), bossEnemy.GetRotation(), rotationLerpRate_ * deltaTime);
	bossEnemy.SetRotation(bossRotation);
}

void BossEnemyIState::EmitTeleportParticle(const Vector3& translation) {

	// 座標の設定
	ParticleSystem::GetInstance()->SetTranslate("bossTeleportEmitter", translation);
	// 発生、更新
	ParticleSystem::GetInstance()->FrequencyEmit("bossTeleportEmitter");
}

void BossEnemyIState::UpdateParticleEmitter(BossEnemyState state) {

	// 状態毎にparticleを更新する
	switch (state) {
	case BossEnemyState::Idle: {

		break;
	}
	case BossEnemyState::Teleport: {

		ParticleSystem::GetInstance()->UpdateEmitter("bossTeleportEmitter");
		break;
	}
	case BossEnemyState::Stun: {

		break;
	}
	case BossEnemyState::Falter: {

		break;
	}
	case BossEnemyState::LightAttack: {

		break;
	}
	case BossEnemyState::StrongAttack: {

		break;
	}
	case BossEnemyState::ChargeAttack: {

		break;
	}
	case BossEnemyState::RushAttack: {

		ParticleSystem::GetInstance()->UpdateEmitter("bossTeleportEmitter");
		break;
	}
	}
}