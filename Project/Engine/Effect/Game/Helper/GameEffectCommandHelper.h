#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Command/ParticleCommand.h>

// front
class GameEffect;

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

	// 姿勢計算と発生を同時に行う
	static void ApplyAndSend(GameEffect& effect, const Quaternion& parentRotation,
		const Vector3& localPos, const Vector3& localEuler);
};