#include "BossEnemyIState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
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

void BossEnemyIState::DrawArc(const Vector3& center, const Vector3& direction,
	float radius, float halfAngle, const Color& color) {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	const float baseYaw = Math::GetYawRadian(direction.Normalize());
	const float halfRadian = pi * halfAngle / 180.0f;

	const int segments = 32;
	Vector3	firstP{};
	Vector3	lastP{};
	for (int i = 0; i < segments; ++i) {

		float t0 = static_cast<float>(i) / segments;
		float t1 = static_cast<float>(i + 1) / segments;

		float a0 = baseYaw - halfRadian + (halfRadian * 2.0f) * t0;
		float a1 = baseYaw - halfRadian + (halfRadian * 2.0f) * t1;

		Vector3 p0(center.x + radius * std::cos(a0), 4.0f, center.z + radius * std::sin(a0));
		Vector3 p1(center.x + radius * std::cos(a1), 4.0f, center.z + radius * std::sin(a1));

		lineRenderer->DrawLine3D(p0, p1, color);

		// playerの座標と結ぶ
		if (i == 0) {
			firstP = p0;
		}
		if (i == segments - 1) {
			lastP = p1;
		}
	}

	lineRenderer->DrawLine3D(center, firstP, Color::Red());
	lineRenderer->DrawLine3D(center, lastP, Color::Red());
}