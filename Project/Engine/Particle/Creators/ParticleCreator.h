#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <list>
// front
class ParticleParameter;
struct ParticleData;

//============================================================================
//	ParticleCreator class
//============================================================================
class ParticleCreator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleCreator() = default;
	~ParticleCreator() = default;

	static void Create(std::list<ParticleData>& particles, const ParticleParameter& parameter);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	static void SetCommonData(ParticleData& particle, const ParticleParameter& parameter);

	static Matrix4x4 CalRotateMatrix(const ParticleParameter& parameter);
};