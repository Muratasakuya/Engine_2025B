#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/MathLib/Vector2.h>
#include <Engine/MathLib/Vector3.h>
#include <Engine/MathLib/Vector4.h>
#include <Engine/MathLib/Quaternion.h>
#include <Engine/MathLib/Matrix4x4.h>

// c++
#include <cstdint>
#include <string>

namespace SakuEngine {

	// front
	class BaseTransform3D;

	//============================================================================
	//	CBufferStructures
	//============================================================================

	/// <summary>
	/// ワールド行列と逆転置行列を保持し、オブジェクト変換をシェーダーへ渡すための定数バッファ構造体。
	/// </summary>
	struct TransformationMatrix {

		Matrix4x4 world;
		Matrix4x4 worldInverseTranspose;

		void Update(const BaseTransform3D* parent, const Vector3& scale,
			const Quaternion& rotation, const Vector3& translation, bool isIgnoreParentScale,
			const std::optional<Matrix4x4>& billboardMatrix = std::nullopt);
	};

	/// <summary>
	/// ベースカラー、テクスチャ参照、発光、UV変換など、3DマテリアルのGPU定数を保持する構造体。
	/// </summary>
	struct MaterialForGPU {

		Color color;

		uint32_t textureIndex;
		uint32_t normalMapTextureIndex;
		int32_t enableNormalMap;
		int32_t enableDithering;

		float emissiveIntensity;
		Vector3 emissionColor;

		Matrix4x4 uvTransform;

		uint32_t postProcessMask;
		uint32_t isRejection;
	};

	/// <summary>
	/// ライティング有効フラグ、反射設定、影係数など、シェーダーのライト計算で使う定数を保持する構造体。
	/// </summary>
	struct LightingForGPU {

		int32_t enableLighting;
		int32_t enableHalfLambert;
		int32_t enableBlinnPhongReflection;
		int32_t enableImageBasedLighting;
		int32_t castShadow;

		float phongRefShininess;
		Vector3 specularColor;

		float shadowRate;
		float environmentCoefficient;
	};

	/// <summary>
	/// スプライト描画で使用する色、UV変換、発光、ポストプロセスマスクをGPUへ渡す構造体。
	/// </summary>
	struct SpriteMaterialForGPU {

		Matrix4x4 uvTransform;
		Color color;
		Vector3 emissionColor;
		int32_t useVertexColor;
		int32_t useAlphaColor;
		float emissiveIntensity;
		float alphaReference;

		// 適応するポストエフェクトのビット
		uint32_t postProcessMask;

		void Init();
		void ImGui();
	};

	/// <summary>
	/// MSDFテキスト描画で使用する色、アウトライン、アトラス情報をGPUへ渡す構造体。
	/// </summary>
	struct MSDFTextMaterialForGPU {

		Color color;
		Color outlineColor;

		Vector2 atlasSize;
		float pixelRange;
		float outlineWidth;

		float softness;
		float boldness;
		uint32_t enableOutline;
		// 適応するポストエフェクトのビット
		uint32_t postProcessMask;

		void Init();
		void ImGui();
	};
}; // SakuEngine
