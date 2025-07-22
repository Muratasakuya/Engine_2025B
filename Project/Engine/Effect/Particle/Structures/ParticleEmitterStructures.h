#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	ParticleEmitterStructures class
//============================================================================

// emitterの形状
enum class ParticleEmitterShape {

	Sphere,
	Hemisphere,
	Box,
	Cone,
	Count,
};

// 球
struct ParticleEmitterSphere {

	float radius;
	Vector3 translation;
};

// 半球
struct ParticleEmitterHemisphere {

	float radius;

	Vector3 translation;
	Matrix4x4 rotationMatrix;
};

// 箱(OBB)
struct ParticleEmitterBox {

	Vector3 size;
	Vector3 translation;
	Matrix4x4 rotationMatrix;
};

// コーン状
struct ParticleEmitterCone {

	float baseRadius;
	float topRadius;
	float height;

	Vector3 translation;
	Matrix4x4 rotationMatrix;
};

struct ParticleEmitterData {

	// 球
	ParticleEmitterSphere sphere;
	// 半球
	ParticleEmitterHemisphere hemiSphere;
	// 箱(OBB)
	ParticleEmitterBox box;
	// コーン状
	ParticleEmitterCone cone;
};

struct ParticleEmitterBufferData {

	// 球
	DxConstBuffer<ParticleEmitterSphere> sphere;
	// 半球
	DxConstBuffer<ParticleEmitterHemisphere> hemisphere;
	// 箱(OBB)
	DxConstBuffer<ParticleEmitterBox> box;
	// コーン状
	DxConstBuffer<ParticleEmitterCone> cone;
};