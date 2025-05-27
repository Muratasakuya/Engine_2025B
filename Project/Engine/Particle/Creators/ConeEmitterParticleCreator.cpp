#include "ConeEmitterParticleCreator.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleEmitter.h>
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	ConeEmitterParticleCreator classMethods
//============================================================================

Vector3 ConeEmitterParticleCreator::CalFacePoint(float radius, float height) {

	float angle = RandomGenerator::Generate(0.0f, 2.0f * pi);
	float radiusRandom = RandomGenerator::Generate(0.0f, radius);
	Vector3 point = Vector3(
		radiusRandom * std::cos(angle),
		height,
		radiusRandom * std::sin(angle));

	return point;
}

void ConeEmitterParticleCreator::Create(ParticleData& particle,
	const ParticleParameter& parameter, const Matrix4x4& rotateMatrix) {

	// 各面の座標を計算
	Vector3 basePoint = CalFacePoint(parameter.emitterCone.baseRadius, 0.0f);
	Vector3 topPoint = CalFacePoint(
		parameter.emitterCone.topRadius, parameter.emitterCone.height);
	Vector3 rotatedBasePoint = rotateMatrix.TransformPoint(basePoint) + parameter.emitterCone.center;
	Vector3 rotatedTopPoint = rotateMatrix.TransformPoint(topPoint) + parameter.emitterCone.center;

	// 回転後の基底点と上面点の間の方向を求める
	Vector3 direction = (rotatedTopPoint - rotatedBasePoint).Normalize();
	// 初期位置を設定
	particle.transform.translation = rotatedBasePoint;
	// 速度方向を設定
	particle.velocity = direction * particle.parameter.moveSpeed.value;
}