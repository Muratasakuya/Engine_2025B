#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Structures/ParticlePrimitiveStructures.h>
#include <Engine/Core/Graphics/GPUObject/DxStructuredBuffer.h>
#include <Engine/Core/Graphics/Lib/DxStructures.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	ParticleStructures
//============================================================================

// パーティクルの種類
enum class ParticleType {

	CPU, // CPUで処理を行う
	GPU, // GPUで処理を行う
	Count
};

// billboardの種類
enum class ParticleBillboardType {

	None,
	All,
	YAxis
};

//============================================================================
//	Common
//============================================================================

namespace ParticleCommon {

	// 描画情報
	template<bool kMultiple = false>
	struct PrimitiveData {

		ParticlePrimitiveType type;

		// 平面
		std::conditional_t<kMultiple, std::vector<PlaneForGPU>, PlaneForGPU> plane;
		// リング
		std::conditional_t<kMultiple, std::vector<RingForGPU>, RingForGPU> ring;
		// 円柱
		std::conditional_t<kMultiple, std::vector<CylinderForGPU>, CylinderForGPU> cylinder;
	};
	struct PrimitiveBufferData {

		ParticlePrimitiveType type;

		// 平面
		DxStructuredBuffer<PlaneForGPU> plane;
		// リング
		DxStructuredBuffer<RingForGPU> ring;
		// 円柱
		DxStructuredBuffer<CylinderForGPU> cylinder;
	};

	struct TransformForGPU {

		Vector3 translation;
		Vector3 scale;
		Matrix4x4 rotationMatrix;

		// 0: full
		// 1: yAxis(XZ回転を適応)
		// 2: none(XYZ回転を適応)
		uint32_t billboardMode;
	};

	struct PerFrameForGPU {

		float time;
		float deltaTime;
		float pad0[2];
	};

	struct PerViewForGPU {

		Vector3 cameraPos;
		float pad0;

		Matrix4x4 viewProjection;
		Matrix4x4 billboardMatrix;
	};
};

//============================================================================
//	GPU
//============================================================================

namespace GPUParticle {

	struct MaterialForGPU {

		Color color;
	};

	struct ParticleForGPU {

		float lifeTime;
		float currentTime;

		Vector3 velocity;
	};
}

//============================================================================
//	CPU
//============================================================================

namespace CPUParticle {

	struct MaterialForGPU {

		Color color;

		// 発光
		float emissiveIntecity;
		Vector3 emissionColor;

		// 閾値
		float alphaReference;
		float noiseAlphaReference;

		Matrix4x4 uvTransform;
	};

	struct TextureInfoForGPU {

		// texture
		uint32_t colorTextureIndex;
		uint32_t noiseTextureIndex;

		// sampler
		// 0...WRAP
		// 1...CLAMP
		uint32_t samplerType;

		// flags
		int32_t useNoiseTexture;
	};

	struct ParticleData {

		// 経過時間
		float currentTime;
		float progress;

		// 移動速度
		Vector3 velocity;
		Vector3 easedVelocity;

		// 回転
		Vector3 rotation;

		// bufferに渡すデータ
		MaterialForGPU material;
		TextureInfoForGPU textureInfo;
		ParticleCommon::TransformForGPU transform;
		ParticleCommon::PrimitiveData<false> primitive;
	};
}