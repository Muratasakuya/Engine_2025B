#include "ParticleCreator.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Particle/ParticleEmitter.h>

// creators
#include <Engine/Particle/Creators/SphereEmitterParticleCreator.h>
#include <Engine/Particle/Creators/HemisphereEmitterParticleCreator.h>
#include <Engine/Particle/Creators/BoxEmitterParticleCreator.h>
#include <Engine/Particle/Creators/ConeEmitterParticleCreator.h>

//============================================================================
//	ParticleCreator classMethods
//============================================================================

void ParticleCreator::Create(std::list<ParticleData>& particles,
	const ParticleParameter& parameter) {

	// 1度でいい処理は1回だけ行う
	Matrix4x4 rotateMatrix = CalRotateMatrix(parameter);

	// particleの数
	const uint32_t emitCount = parameter.emitCount.GetValue();
	// emitCountの分だけ作成する
	for (uint32_t index = 0; index < emitCount; ++index) {

		// 各関数に渡して作成する
		// 参照渡しで値を入れていく
		ParticleData particle{};
		particle.material.Init();

		// 共通の値を設定
		SetCommonData(particle, parameter);

		// emitterType別で必要な値を作成する
		switch (parameter.emitterShape) {
		case EmitterShapeType::Sphere: {

			SphereEmitterParticleCreator::Create(particle, parameter);
			break;
		}
		case EmitterShapeType::Hemisphere: {

			HemisphereEmitterParticleCreator::Create(particle, parameter, rotateMatrix);
			break;
		}
		case EmitterShapeType::Box: {

			BoxEmitterParticleCreator::Create(particle, parameter, rotateMatrix);
			break;
		}
		case EmitterShapeType::Cone: {

			ConeEmitterParticleCreator::Create(particle, parameter, rotateMatrix);
			break;
		}
		}

		// particle追加
		particles.emplace_back(particle);
	}
}

void ParticleCreator::SetCommonData(ParticleData& particle, const ParticleParameter& parameter) {

	//============================================================================
	//	初期化値設定: 定数値

	// 時間
	particle.currentTime = 0.0f;
	particle.progress = 0.0f;

	//============================================================================
	//	初期化値設定: ランダム値

	// 発生
	particle.parameter.lifeTime.value = parameter.lifeTime.GetValue();
	// 移動
	particle.parameter.moveSpeed.value = parameter.moveSpeed.GetValue();

	// スケール
	particle.parameter.startScale.value = parameter.startScale.GetValue();
	particle.parameter.targetScale.value = parameter.targetScale.GetValue();

	// 回転
	particle.parameter.startRotationMultiplier.value = parameter.startRotationMultiplier.GetValue();
	particle.parameter.targetRotationMultiplier.value = parameter.targetRotationMultiplier.GetValue();

	// 色
	particle.parameter.startColor.value = parameter.startColor.GetValue();
	particle.parameter.targetColor.value = parameter.targetColor.GetValue();

	// 発光
	// 強度
	particle.parameter.startEmissiveIntensity.value = parameter.startEmissiveIntensity.GetValue();
	particle.parameter.targetEmissiveIntensity.value = parameter.targetEmissiveIntensity.GetValue();
	// 色
	particle.parameter.startEmissionColor.value = parameter.startEmissionColor.GetValue();
	particle.parameter.targetEmissionColor.value = parameter.targetEmissionColor.GetValue();

	// alphaReference
	// texture
	particle.parameter.startTextureAlphaReference.value = parameter.startTextureAlphaReference.GetValue();
	particle.parameter.targetTextureAlphaReference.value = parameter.targetTextureAlphaReference.GetValue();
	// noise
	particle.parameter.startNoiseTextureAlphaReference.value = parameter.startNoiseTextureAlphaReference.GetValue();
	particle.parameter.targetNoiseTextureAlphaReference.value = parameter.targetNoiseTextureAlphaReference.GetValue();

	// noiseDiscard
	// エッジ
	particle.parameter.startEdgeSize.value = parameter.startEdgeSize.GetValue();
	particle.parameter.targetEdgeSize.value = parameter.targetEdgeSize.GetValue();
	particle.parameter.startEdgeColor.value = parameter.startEdgeColor.GetValue();
	particle.parameter.targetEdgeColor.value = parameter.targetEdgeColor.GetValue();
	// 発光
	particle.parameter.startEdgeEmissiveIntensity.value = parameter.startEdgeEmissiveIntensity.GetValue();
	particle.parameter.targetEdgeEmissiveIntensity.value = parameter.targetEdgeEmissiveIntensity.GetValue();
	particle.parameter.startEdgeEmissionColor.value = parameter.startEdgeEmissionColor.GetValue();
	particle.parameter.targetEdgeEmissionColor.value = parameter.targetEdgeEmissionColor.GetValue();

	// uv
	// スケール
	particle.parameter.startUVScale.value = parameter.startUVScale.GetValue();
	particle.parameter.targetUVScale.value = parameter.targetUVScale.GetValue();
	// Z回転
	particle.parameter.startUVRotationZ.value = parameter.startUVRotationZ.GetValue();
	particle.parameter.targetUVRotationZ.value = parameter.targetUVRotationZ.GetValue();
	// スクロール
	particle.parameter.startUVTranslation.value = parameter.startUVTranslation.GetValue();
	particle.parameter.targetUVTranslation.value = parameter.targetUVTranslation.GetValue();

	// 物理
	particle.parameter.reflectFace = parameter.reflectFace;
	particle.parameter.restitution.value = parameter.restitution.GetValue();
	particle.parameter.gravityStrength.value = parameter.gravityStrength.GetValue();
	particle.parameter.gravityDirection.value = parameter.gravityDirection.GetValue();
}

Matrix4x4 ParticleCreator::CalRotateMatrix(const ParticleParameter& parameter) {

	// game内で使うか使わないかで回転を分岐
	Matrix4x4 result = Matrix4x4::MakeIdentity4x4();
	if (parameter.IsUseGame()) {

		// 回転行列を計算
		if (parameter.emitterShape == EmitterShapeType::Hemisphere) {

			result = Quaternion::MakeRotateMatrix(parameter.emitterHemisphere.rotation);
		} else if (parameter.emitterShape == EmitterShapeType::Box) {

			result = Quaternion::MakeRotateMatrix(parameter.emitterBox.rotation);
		} else if (parameter.emitterShape == EmitterShapeType::Cone) {

			result = Quaternion::MakeRotateMatrix(parameter.emitterCone.rotation);
		}
	} else {

		// 回転行列を計算
		if (parameter.emitterShape == EmitterShapeType::Hemisphere) {

			result = Matrix4x4::MakeRotateMatrix(parameter.emitterHemisphere.eulerRotate);
		} else if (parameter.emitterShape == EmitterShapeType::Box) {

			result = Matrix4x4::MakeRotateMatrix(parameter.emitterBox.eulerRotate);
		} else if (parameter.emitterShape == EmitterShapeType::Cone) {

			result = Matrix4x4::MakeRotateMatrix(parameter.emitterCone.eulerRotate);
		}
	}

	return result;
}