#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	SphereEmitterParticleCreator class
//============================================================================
class SphereEmitterParticleCreator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SphereEmitterParticleCreator() = default;
	~SphereEmitterParticleCreator() = default;

	static void Create(ParticleData& particle, const ParticleParameter& parameter);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	static Vector3 RandomFibonacciDirection();
};