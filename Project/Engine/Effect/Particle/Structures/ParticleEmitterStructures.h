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

struct ParticleEmitterCommon {

	int32_t count;
	int32_t emit;
	float lifeTime;

	Vector3 scale;

	Color color;

	void Init() {

		// 初期値
		count = 32;
		emit = false;
		lifeTime = 1.0f;

		scale = Vector3::AnyInit(0.4f);
		color = Color::White();
	}
};

// 球
struct ParticleEmitterSphere {

	float radius;
	Vector3 translation;

	void Init() {

		radius = 2.0f;
		translation = Vector3::AnyInit(0.0f);
	}
};

// 半球
struct ParticleEmitterHemisphere {

	float radius;

	Vector3 translation;
	Matrix4x4 rotationMatrix;

	void Init() {

		radius = 2.0f;
		translation = Vector3::AnyInit(0.0f);
		rotationMatrix = Matrix4x4::MakeIdentity4x4();
	}
};

// 箱(OBB)
struct ParticleEmitterBox {

	Vector3 size;
	Vector3 translation;
	Matrix4x4 rotationMatrix;

	void Init() {

		size = Vector3::AnyInit(2.0f);
		translation = Vector3::AnyInit(0.0f);
		rotationMatrix = Matrix4x4::MakeIdentity4x4();
	}
};

// コーン状
struct ParticleEmitterCone {

	float baseRadius;
	float topRadius;
	float height;

	Vector3 translation;
	Matrix4x4 rotationMatrix;

	void Init() {

		baseRadius = 0.4f;
		topRadius = 1.6f;
		height = 1.6f;

		translation = Vector3::AnyInit(0.0f);
		rotationMatrix = Matrix4x4::MakeIdentity4x4();
	}
};

struct ParticleEmitterData {

	ParticleEmitterShape shape;

	// 発生
	ParticleEmitterCommon common;

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

	// 発生
	DxConstBuffer<ParticleEmitterCommon> common;

	// 球
	DxConstBuffer<ParticleEmitterSphere> sphere;
	// 半球
	DxConstBuffer<ParticleEmitterHemisphere> hemisphere;
	// 箱(OBB)
	DxConstBuffer<ParticleEmitterBox> box;
	// コーン状
	DxConstBuffer<ParticleEmitterCone> cone;
};