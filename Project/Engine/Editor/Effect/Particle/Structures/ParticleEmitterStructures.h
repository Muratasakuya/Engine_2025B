#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessBit.h>
#include <Engine/MathLib/MathUtils.h>

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

// パーティクルの削除方法
enum class ParticleDeleteMode {

	LifeTime,   // ライフタイムで削除
	ColorAlpha, // 色のアルファ値で削除
};

namespace SakuEngine {

	/// <summary>
	/// エミッター共通の発生数、寿命、速度、スケール、色、ポストエフェクト対象を保持する。
	/// </summary>
	struct ParticleEmitterCommon {

		int32_t count;
		int32_t emit;
		float lifeTime;
		float moveSpeed;

		Vector3 scale;
		float pad1;

		Color color;

		// 適応するポストエフェクトのビット
		uint32_t postProcessMask;
		void Init() {

			// 初期値
			count = 32;
			emit = false;
			lifeTime = 1.0f;
			moveSpeed = 1.0f;

			scale = SakuEngine::Vector3::AnyInit(0.4f);
			color = SakuEngine::Color::White();

			// デフォルトでかけるポストプロセス
			postProcessMask = Bit_Bloom | Bit_RadialBlur | Bit_Glitch | Bit_Vignette;
		}
	};

	/// <summary>
	/// 球形エミッターの半径と中心位置を保持し、球面/球内部からの発生範囲を定義する。
	/// </summary>
	struct ParticleEmitterSphere {

		float radius;

		Vector3 translation;
		float pad0;

		void Init() {

			radius = 2.0f;
			translation = SakuEngine::Vector3::AnyInit(0.0f);
		}
	};

	/// <summary>
	/// 半球エミッターの半径、位置、向きを保持し、片側方向へ広がる発生範囲を定義する。
	/// </summary>
	struct ParticleEmitterHemisphere {

		float radius;

		Vector3 translation;
		Matrix4x4 rotationMatrix;

		void Init() {

			radius = 2.0f;
			translation = SakuEngine::Vector3::AnyInit(0.0f);
			rotationMatrix = Matrix4x4::MakeIdentity4x4();
		}
	};

	/// <summary>
	/// 箱形エミッターのサイズ、位置、回転を保持し、OBB領域からの発生範囲を定義する。
	/// </summary>
	struct ParticleEmitterBox {

		Vector3 size;
		float pod0;

		Vector3 translation;
		float pod1;

		Matrix4x4 rotationMatrix;

		void Init() {

			size = SakuEngine::Vector3::AnyInit(2.0f);
			translation = SakuEngine::Vector3::AnyInit(0.0f);
			rotationMatrix = Matrix4x4::MakeIdentity4x4();
		}
	};

	/// <summary>
	/// 円錐台エミッターの上下半径、高さ、位置、回転を保持し、円錐状の発生範囲を定義する。
	/// </summary>
	struct ParticleEmitterCone {

		float baseRadius;
		float topRadius;
		float height;
		float pod0;

		Vector3 translation;
		float pod1;

		Matrix4x4 rotationMatrix;

		void Init() {

			baseRadius = 0.4f;
			topRadius = 1.6f;
			height = 1.6f;

			translation = SakuEngine::Vector3::AnyInit(0.0f);
			rotationMatrix = Matrix4x4::MakeIdentity4x4();
		}
	};

	/// <summary>
	/// 選択中のエミッター形状と、各形状別パラメータをまとめて保持する編集/実行用データ。
	/// </summary>
	struct ParticleEmitterData {

		ParticleEmitterShape shape;

		// 発生
		ParticleEmitterCommon common;

		// 球
		ParticleEmitterSphere sphere;
		// 半球
		ParticleEmitterHemisphere hemisphere;
		// 箱(OBB)
		ParticleEmitterBox box;
		// コーン状
		ParticleEmitterCone cone;

		void Init() {

			// 全て初期化
			sphere.Init();
			hemisphere.Init();
			box.Init();
			cone.Init();
		}
	};

	/// <summary>
	/// エミッター共通情報と形状別情報をGPUへ転送するための定数バッファ群。
	/// </summary>
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

}; // SakuEngine
