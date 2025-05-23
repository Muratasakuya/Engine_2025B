#include "BoxEmitterParticleCreator.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleEmitter.h>
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	BoxEmitterParticleCreator classMethods
//============================================================================

void BoxEmitterParticleCreator::Create(ParticleData& particle, const ParticleParameter& parameter,
	const Matrix4x4& rotateMatrix) {

	// ローカルBox内でランダムな座標を設定
	Vector3 halfSize = parameter.emitterBox.size * 0.5f;
	Vector3 localPos = RandomGenerator::Generate(
		Vector3(-halfSize.x, -halfSize.y, -halfSize.z), halfSize);

	// ワールド座標に変換し設定
	particle.transform.translation = rotateMatrix.TransformPoint(localPos) + parameter.emitterBox.center;

	// 向いている方向(+Z)方向に飛ばす
	Vector3 forward = Vector3::Normalize(Vector3::TransferNormal(Vector3(0.0f, 0.0f, 1.0f), rotateMatrix));
	particle.velocity = forward * particle.parameter.moveSpeed.value;
}