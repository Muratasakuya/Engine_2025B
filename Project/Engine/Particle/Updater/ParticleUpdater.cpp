#include "ParticleUpdater.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleEmitter.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	ParticleUpdater classMethods
//============================================================================

void ParticleUpdater::Update(ParticleData& particle,
	const ParticleParameter& parameter, const Matrix4x4& billboardMatrix) {

	// 時間の更新処理
	float deltaTime = 0.0f;
	UpdateTime(deltaTime, particle, parameter);

	// イージング後の速度を計算
	particle.easedVelocity = particle.velocity * particle.easedProgressT;
	// SRTの更新処理
	UpdateScale(particle, parameter);
	UpdateRotate(deltaTime, particle, parameter);
	UpdateTranslate(deltaTime, particle, parameter);
	// worldMatrixの更新処理
	UpdateMatrix(particle, parameter, billboardMatrix);

	// materialの更新
	UpdateMaterial(particle, parameter);
}

void ParticleUpdater::UpdateTime(float& deltaTime, ParticleData& particle, const ParticleParameter& parameter) {

	// deltaTimeの分岐
	if (parameter.useScaledTime) {
		deltaTime = GameTimer::GetScaledDeltaTime();
	} else {
		deltaTime = GameTimer::GetDeltaTime();
	}
	// 時間を進める
	particle.currentTime += deltaTime;

	// 進行度計算
	float progress = particle.currentTime / particle.parameter.lifeTime.value;
	// イージング値
	particle.easedProgressT = EasedValue(parameter.easingType, progress);
}

void ParticleUpdater::UpdateScale(ParticleData& particle, [[maybe_unused]] const ParticleParameter& parameter) {

	// targetに向けて補間する
	particle.transform.scale = Vector3::Lerp(
		particle.parameter.startScale.value,
		particle.parameter.targetScale.value, particle.easedProgressT);
}

void ParticleUpdater::UpdateRotate(float deltaTime, ParticleData& particle, const ParticleParameter& parameter) {

	// 進行方向に回転を設定する
	if (parameter.moveToDirection && parameter.billboardType == ParticleBillboardType::None) {

		// 進行方向を取得
		Vector3 direction = particle.easedVelocity.Normalize();
		particle.transform.eulerRotate.y = std::atan2(direction.x, direction.z);
		// 横軸方向の長さを求める
		float horizontalLength = std::sqrtf(direction.x * direction.x + direction.z * direction.z);
		// X軸周りの角度(θx)
		particle.transform.eulerRotate.x = std::atan2(-direction.y, horizontalLength);
	} else {

		// 回転量をtargetに向けて補間
		Vector3 rotationMultiplier = Vector3::Lerp(
			particle.parameter.startRotationMultiplier.value,
			particle.parameter.targetRotationMultiplier.value, particle.easedProgressT);
		// 回転量を足す
		particle.transform.eulerRotate += rotationMultiplier * deltaTime;
	}
}

void ParticleUpdater::UpdateTranslate(float deltaTime,
	ParticleData& particle, const ParticleParameter& parameter) {

	// 壁に反射するかどうか
	if (parameter.reflectGround) {

		Vector3 newPosition = particle.transform.translation + particle.easedVelocity * deltaTime;

		// 衝突判定
		if (newPosition.y <= particle.parameter.reflectFace.y && particle.velocity.y < 0.0f) {

			// 反射処理
			particle.velocity = Vector3::Reflect(particle.velocity, Vector3(0.0f, 1.0f, 0.0f)) * particle.parameter.restitution.value;

			// 反射位置よりも進めないように調整
			newPosition.y = particle.parameter.reflectFace.y;
		}

		// 更新された位置に適用
		particle.transform.translation = newPosition;
	} else {

		// 座標を加算
		particle.transform.translation += particle.easedVelocity * deltaTime;
	}

	// 重力の適応
	Vector3 gravityEffect =
		particle.parameter.gravityDirection.value * particle.parameter.gravityStrength.value * deltaTime;
	particle.velocity += gravityEffect;
}

void ParticleUpdater::UpdateMatrix(ParticleData& particle,
	const ParticleParameter& parameter, const Matrix4x4& billboardMatrix) {

	// STの行列計算
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(particle.transform.scale);
	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(particle.transform.translation);

	// worldMatrixの更新処理、billboardTypeで分岐
	// 全軸
	switch (parameter.billboardType) {
	case ParticleBillboardType::None: {

		// billboard無し
		particle.transform.matrix.world = Matrix4x4::MakeAffineMatrix(
			particle.transform.scale, particle.transform.eulerRotate, particle.transform.translation);
		break;
	}
	case ParticleBillboardType::All: {

		// 全軸
		particle.transform.matrix.world = scaleMatrix * billboardMatrix * translateMatrix;
		break;
	}
	case ParticleBillboardType::YAxis: {

		// Y軸
		break;
	}
	}
}

void ParticleUpdater::UpdateMaterial(ParticleData& particle, const ParticleParameter& parameter) {

	// texture情報を渡す
	particle.material.textureIndex = parameter.textureIndex;
	particle.material.noiseTextureIndex = parameter.noiseTextureIndex;

	// 色をtargetに向けて補間
	particle.material.color = Color::Lerp(
		particle.parameter.startColor.value,
		particle.parameter.targetColor.value, particle.easedProgressT);
	// 頂点色を使用する場合targetに向けて補間
	// 設計ミスったので後回し...
	particle.material.useVertexColor = parameter.useVertexColor;

	// 発光
	// 強度、targetに向けて補間
	particle.material.emissiveIntensity = std::lerp(
		particle.parameter.startEmissiveIntensity.value,
		particle.parameter.targetEmissiveIntensity.value, particle.easedProgressT);
	// 色、targetに向けて補間
	particle.material.emissionColor = Vector3::Lerp(
		particle.parameter.startEmissionColor.value,
		particle.parameter.targetEmissionColor.value, particle.easedProgressT);

	// alphaReference、targetに向けて補間
	// texture
	particle.material.textureAlphaReference = std::lerp(
		particle.parameter.startTextureAlphaReference.value,
		particle.parameter.targetTextureAlphaReference.value, particle.easedProgressT);
	// noiseTextureを使用する場合のみ処理を行う
	particle.material.useNoiseTexture = parameter.useNoiseTexture;
	if (particle.material.useNoiseTexture) {

		particle.material.noiseTextureAlphaReference = std::lerp(
			particle.parameter.startNoiseTextureAlphaReference.value,
			particle.parameter.targetNoiseTextureAlphaReference.value, particle.easedProgressT);
	}
}