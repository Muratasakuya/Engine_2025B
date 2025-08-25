#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Command/ParticleCommand.h>

// front
class GameEffect;
class BaseCamera;

//============================================================================
//	GameEffectCommandHelper class
//============================================================================
class GameEffectCommandHelper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameEffectCommandHelper() = default;
	~GameEffectCommandHelper() = default;

	// 発生の設定
	static void SendSpawnerEmit(GameEffect& effect, bool emit);

	// ビルボードを発生モジュールに設定
	static void SendSpawnerBillboard(GameEffect& effect,
		const BaseCamera& camera);

	// 姿勢計算と発生を同時に行う
	// 座標
	static void ApplyAndSend(GameEffect& effect, const Quaternion& parentRotation,
		const Vector3& localPos);
	// 回転と座標
	static void ApplyAndSend(GameEffect& effect, const Quaternion& parentRotation,
		const Vector3& localPos, const Vector3& localEuler);
};