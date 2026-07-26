#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Effect/Particle/Structures/ParticlePrimitiveStructures.h>
#include <Engine/Editor/Effect/Particle/Structures/ParticleValue.h>
#include <Engine/Editor/Effect/Particle/Structures/ParticleEmitterStructures.h>
#include <Engine/Core/Graphics/GPUObject/DxStructuredBuffer.h>
#include <Engine/Core/Graphics/DxLib/DxStructures.h>
#include <Engine/Object/Data/Transform/Transform.h>
#include <Engine/Utility/Enum/Easing.h>
#include <Engine/MathLib/MathUtils.h>

// c++
#include <deque>

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

	All,
	YAxis,
	None
};

//============================================================================
//	Common
//============================================================================

namespace SakuEngine {

namespace ParticleCommon {

	// samplerの種類
	enum class SamplerType {

		WRAP,
		CLMAP
	};

	template<bool kMultiple = false>
	/// <summary>
	/// パーティクルのプリミティブ種別と、CPU/GPU転送前の形状パラメータを保持する。
	/// </summary>
	struct PrimitiveData {

		ParticlePrimitiveType type;

		// 平面
		std::conditional_t<kMultiple, std::vector<PlaneForGPU>, PlaneForGPU> plane;
		// リング
		std::conditional_t<kMultiple, std::vector<RingForGPU>, RingForGPU> ring;
		// 円柱
		std::conditional_t<kMultiple, std::vector<CylinderForGPU>, CylinderForGPU> cylinder;
		// 三日月
		std::conditional_t<kMultiple, std::vector<CrescentForGPU>, CrescentForGPU> crescent;
		// 雷
		std::conditional_t<kMultiple, std::vector<LightningForGPU>, LightningForGPU> lightning;
		// テストメッシュ
		std::conditional_t<kMultiple, std::vector<TestMeshForGPU>, TestMeshForGPU> testMesh;
	};
	/// <summary>
	/// プリミティブ描画用データをGPUへ送るためのStructuredBuffer群。
	/// </summary>
	struct PrimitiveBufferData {

		ParticlePrimitiveType type;

		// 平面
		DxStructuredBuffer<PlaneForGPU> plane;
		// リング
		DxStructuredBuffer<RingForGPU> ring;
		// 円柱
		DxStructuredBuffer<CylinderForGPU> cylinder;
		// 三日月
		DxStructuredBuffer<CrescentForGPU> crescent;
		// 雷
		DxStructuredBuffer<LightningForGPU> lightning;
		// テストメッシュ
		DxStructuredBuffer<TestMeshForGPU> testMesh;
	};

	/// <summary>
	/// パーティクルのワールド変換、親行列、ビルボード設定をGPUへ渡す構造体。
	/// </summary>
	struct TransformForGPU {

		Vector3 translation;
		Vector3 scale;
		Matrix4x4 rotationMatrix;
		Matrix4x4 parentMatrix;

		// 0: full
		// 1: yAxis(XZ回転を適応)
		// 2: none(XYZ回転を適応)
		uint32_t billboardMode;

		// 0: 親無し
		// 1: 親有り
		uint32_t aliveParent = false;
	};

	/// <summary>
	/// パーティクル更新で毎フレーム共有する時刻とデルタタイムを保持する。
	/// </summary>
	struct PerFrameForGPU {

		float time;
		float deltaTime;
		float pad0[2];
	};

	/// <summary>
	/// パーティクル描画で参照するカメラ位置、ビュー射影、ビルボード行列を保持する。
	/// </summary>
	struct PerViewForGPU {

		Vector3 cameraPos;
		float pad0;

		Matrix4x4 viewProjection;
		Matrix4x4 billboardMatrix;
	};

	template <typename T>
	/// <summary>
	/// 線形補間で使用する開始値と目標値の組。
	/// </summary>
	struct LerpValue {

		T start;
		T target;
	};

	template <typename T>
	/// <summary>
	/// エディター入力用の可変値として補間開始値と目標値を保持する。
	/// </summary>
	struct EditLerpValue {

		ParticleValue<T> start;
		ParticleValue<T> target;
	};

	/// <summary>
	/// トレイルを構成する1サンプルのワールド位置と経過時間を保持する。
	/// </summary>
	struct TrailPoint {

		Vector3 pos; // 座標
		float age;   // 寿命
	};
	/// <summary>
	/// トレイルのサンプル列、前回位置、追従解除状態など実行時の履歴を保持する。
	/// </summary>
	struct TrailRuntime {

		std::deque<TrailPoint> nodes; // リングバッファ
		Vector3 prePos;               // 前回のサンプル位置

		bool isInitialized = false;   // 初期化済みか
		float time;                   // サンプル間隔
		bool isDetaching = false;     // 追従先が消えた後の処理を行うか
	};

	/// <summary>
	/// トレイル頂点バッファ内で使用する開始位置と頂点数をGPUへ渡すヘッダー。
	/// </summary>
	struct TrailHeaderForGPU {

		uint32_t start;
		uint32_t vertexCount;
	};
	/// <summary>
	/// トレイルメッシュ1頂点分のワールド位置、UV、色を保持するGPU転送用データ。
	/// </summary>
	struct TrailVertexForGPU {

		Vector3 worldPos;
		Vector2 uv;
		Color color;
	};
	/// <summary>
	/// トレイル描画で使用するテクスチャ参照、サンプラー種別、ノイズ使用フラグを保持する。
	/// </summary>
	struct TrailTextureInfoForGPU {

		// texture
		uint32_t colorTextureIndex;
		uint32_t noiseTextureIndex;

		// sampler
		// 0...WRAP
		// 1...CLAMP
		int32_t samplerType;

		// flags
		int32_t useNoiseTexture;
	};
};

//============================================================================
//	GPU
//============================================================================

namespace GPUParticle {

	// 更新の種類
	enum class UpdateType {

		None,
		Noise,
		Count
	};

	/// <summary>
	/// GPUパーティクルのマテリアル色とポストエフェクト対象を保持する。
	/// </summary>
	struct MaterialForGPU {

		Color color;

		// 適応するポストエフェクトのビット
		uint32_t postProcessMask;
	};

	/// <summary>
	/// GPU更新パーティクルの寿命、経過時間、速度を保持する。
	/// </summary>
	struct ParticleForGPU {

		float lifeTime;
		float currentTime;

		Vector3 velocity;
	};

	/// <summary>
	/// GPUパーティクルをノイズで動かす際のスケール、強さ、速度を保持する。
	/// </summary>
	struct NoiseForGPU {

		float scale;
		float strength;
		float speed;

		void Init() {

			scale = 0.04f;
			strength = 1.0f;
			speed = 0.1f;
		}
	};
}

//============================================================================
//	CPU
//============================================================================

namespace CPUParticle {

	/// <summary>
	/// CPUパーティクル描画の色、発光、アルファしきい値、UV変換、ポストエフェクト対象を保持する。
	/// </summary>
	struct MaterialForGPU {

		Color color;

		// 発光
		float emissiveIntecity;
		Vector3 emissionColor;

		// 閾値
		float alphaReference;
		float noiseAlphaReference;

		Matrix4x4 colorUVTransform = Matrix4x4::MakeIdentity4x4();
		Matrix4x4 noiseUVTransform = Matrix4x4::MakeIdentity4x4();

		// 適応するポストエフェクトのビット
		uint32_t postProcessMask;
	};

	/// <summary>
	/// CPUパーティクル描画で使用する色/ノイズテクスチャ、サンプラー、使用フラグを保持する。
	/// </summary>
	struct TextureInfoForGPU {

		// texture
		uint32_t colorTextureIndex;
		uint32_t noiseTextureIndex;

		// sampler
		// 0...WRAP
		// 1...CLAMP
		int32_t samplerType;

		// flags
		int32_t useNoiseTexture;
	};

	/// <summary>
	/// CPUパーティクル1個分の寿命、移動、トレイル、描画、削除方式など実行時状態を保持する。
	/// </summary>
	struct ParticleData {

		// 生存時間
		float lifeTime;

		// 経過時間
		float currentTime;
		float progress;
		// 現在のフェーズ
		uint32_t phaseIndex;
		// 発生したときのインデックス
		uint32_t spawnIndex;

		// キー補間の開始したときの補間t
		float keyPathStartT = -1.0f;
		bool hasKeyPathStart = false;
		float keyPathSpawnAngle = 0.0f;
		bool hasKeyPathSpawnAngle = false;

		// 発生したときの座標
		Vector3 spawnTranlation;
		// 回転の保持
		Quaternion rotation;

		// 発生させたエミッターの座標
		Vector3 emitterTranslation;

		// トレイル
		ParticleCommon::TrailRuntime trailRuntime;

		// bufferを更新するデータ
		// 移動速度
		Vector3 velocity;

		// 削除方法
		ParticleDeleteMode deleteMode;

		// bufferに渡すデータ
		MaterialForGPU material;
		TextureInfoForGPU textureInfo;
		ParticleCommon::TrailTextureInfoForGPU trailTextureInfo;
		ParticleCommon::TransformForGPU transform;
		ParticleCommon::PrimitiveData<false> primitive;
	};
}

}; // SakuEngine
