#include "HemisphereEmitterParticleCreator.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleEmitter.h>
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	HemisphereEmitterParticleCreator classMethods
//============================================================================

Vector3 HemisphereEmitterParticleCreator::RandomFibonacciDirection() {

	float phi = RandomGenerator::Generate(0.0f, 2.0f * pi);
	float z = RandomGenerator::Generate(-1.0f, 1.0f);
	float sqrtOneMinusZ2 = std::sqrt(1.0f - z * z);
	Vector3 result = Vector3(sqrtOneMinusZ2 * std::cos(phi), sqrtOneMinusZ2 * std::sin(phi), z).Normalize();
	return result;
}


void HemisphereEmitterParticleCreator::Create(ParticleData& particle,
	const ParticleParameter& parameter, const Matrix4x4& rotateMatrix) {

	// 球状にランダムな向きを設定
	Vector3 direction = RandomFibonacciDirection();
	// 半球方向限定
	if (direction.y < 0.0f) {
		direction.y = -direction.y;
	}
	// 回転を考慮して設定
	Vector3 rotatedDirection = rotateMatrix.TransformPoint(direction);
	// 初期位置を設定
	particle.transform.translation = parameter.emitterSphere.center + direction * parameter.emitterSphere.radius;
	// 速度方向を設定
	particle.velocity = rotatedDirection * particle.parameter.moveSpeed.GetValue();
}