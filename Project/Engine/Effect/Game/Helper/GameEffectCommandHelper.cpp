#include "GameEffectCommandHelper.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/GameEffect.h>

//============================================================================
//	GameEffectCommandHelper classMethods
//============================================================================

void GameEffectCommandHelper::ApplyAndSend(GameEffect& effect,
	const Quaternion& parentRotation, const Vector3& localPos) {

	// 回転を考慮した発生位置を計算
	const Vector3 emitPos = Quaternion::RotateVector(localPos, parentRotation);

	ParticleCommand command{};
	command.target = ParticleCommandTarget::Spawner;
	command.id = ParticleCommandID::SetTranslation;
	command.value = emitPos;
	effect.SendCommand(command);
}

void GameEffectCommandHelper::ApplyAndSend(GameEffect& effect, const Quaternion& parentRotation,
	const Vector3& localPos, const Vector3& localEuler) {

	// ローカル回転を計算
	const Quaternion localRotation = Quaternion::EulerToQuaternion(localEuler);
	const Vector3 forwardLocal = Quaternion::RotateVector(Vector3(0.0f, 0.0f, 1.0f), localRotation);
	const Vector3 upLocal = Quaternion::RotateVector(Vector3(0.0f, 1.0f, 0.0f), localRotation);

	// 親の回転でワールド方向を求める
	const Vector3 forwardWorld = Quaternion::RotateVector(forwardLocal, parentRotation);
	const Vector3 upWorld = Quaternion::RotateVector(upLocal, parentRotation);

	// ワールド回転を計算
	const Quaternion worldRotateMatrix = Quaternion::LookRotation(forwardWorld, upWorld);
	const Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(worldRotateMatrix);

	// 回転を考慮した発生位置を計算
	const Vector3 emitPos = Quaternion::RotateVector(localPos, parentRotation);

	// コマンドを設定
	{
		ParticleCommand command{};
		command.target = ParticleCommandTarget::Updater;
		command.id = ParticleCommandID::SetEulerRotation;
		command.filter.updaterId = ParticleUpdateModuleID::Rotation;
		command.value = rotateMatrix;
		effect.SendCommand(command);
	}
	{
		ParticleCommand command{};
		command.target = ParticleCommandTarget::Spawner;
		command.id = ParticleCommandID::SetTranslation;
		command.value = emitPos;
		effect.SendCommand(command);
	}
}