#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Matrix4x4.h>

// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	ParticleUpdater class
//============================================================================
class ParticleUpdater {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleUpdater() = default;
	~ParticleUpdater() = default;

	static void Update(ParticleData& particle,
		const ParticleParameter& parameter, const Matrix4x4& billboardMatrix);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// 時間の更新
	static void UpdateTime(float& deltaTime, ParticleData& particle, const ParticleParameter& parameter);
	// SRTの更新
	static void UpdateScale(ParticleData& particle, const ParticleParameter& parameter);
	static void UpdateRotate(float deltaTime, ParticleData& particle, const ParticleParameter& parameter);
	static void UpdateTranslate(float deltaTime, ParticleData& particle, const ParticleParameter& parameter);
	// worldMatrixの更新
	static void UpdateMatrix(ParticleData& particle,
		const ParticleParameter& parameter, const Matrix4x4& billboardMatrix);
	// materialの更新
	static void UpdateMaterial(ParticleData& particle, const ParticleParameter& parameter);
};