#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	HemisphereEmitterParticleCreator class
//============================================================================
class HemisphereEmitterParticleCreator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	HemisphereEmitterParticleCreator() = default;
	~HemisphereEmitterParticleCreator() = default;

	static void Create(ParticleData& particle, const ParticleParameter& parameter, const Matrix4x4& rotateMatrix);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	static Vector3 RandomFibonacciDirection();
};