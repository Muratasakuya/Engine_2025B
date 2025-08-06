#include "BossEnemyIState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

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

	translation;

	// 座標の設定
	// システム変更で消えた
	// 発生、更新
	// システム変更で消えた
}

void BossEnemyIState::UpdateParticleEmitter(BossEnemyState state) {

	// 状態毎にparticleを更新する
	switch (state) {
	case BossEnemyState::Idle: {

		break;
	}
	case BossEnemyState::Teleport: {

		// システム変更で消えた
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

		// システム変更で消えた
		break;
	}
	}
}