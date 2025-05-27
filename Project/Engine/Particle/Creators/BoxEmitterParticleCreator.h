#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	BoxEmitterParticleCreator class
//============================================================================
class BoxEmitterParticleCreator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BoxEmitterParticleCreator() = default;
	~BoxEmitterParticleCreator() = default;

	static void Create(ParticleData& particle, const ParticleParameter& parameter, const Matrix4x4& rotateMatrix);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

};