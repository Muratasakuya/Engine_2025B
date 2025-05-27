#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	ConeEmitterParticleCreator class
//============================================================================
class ConeEmitterParticleCreator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ConeEmitterParticleCreator() = default;
	~ConeEmitterParticleCreator() = default;

	static void Create(ParticleData& particle, const ParticleParameter& parameter, const Matrix4x4& rotateMatrix);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	static Vector3 CalFacePoint(float radius, float height);
};