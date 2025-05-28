#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/DxStructures.h>
#include <Engine/Particle/Structures/EmitterStructures.h>
#include <Engine/Particle/Structures/ParticleValue.h>
#include <Lib/Adapter/Easing.h>

// front
class Asset;

//============================================================================
//	ParticleParameter class
//============================================================================
class ParticleParameter {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleParameter() = default;
	~ParticleParameter() = default;

	//--------- functions ----------------------------------------------------

	// 初期値設定
	void Init(std::string name, const std::string& modelName, Asset* asset);

	void ImGui();

	// 保存操作
	void SaveJson(const std::string& saveName);

	//--------- variables ----------------------------------------------------

	// 描画ブレンドモード
	BlendMode blendMode;

	// emitterの形状
	EmitterShapeType emitterShape;
	// emitterの形状データ
	EmitterSphere emitterSphere;
	EmitterHemisphere emitterHemisphere;
	EmitterBox emitterBox;
	EmitterCone emitterCone;

	// billboardの種類
	ParticleBillboardType billboardType;

	// easingの種類
	EasingType easingType;

	// parameter
	ParticleValue<uint32_t> emitCount; // 1度に発生させる数
	float frequency;                   // ~秒置き、発生頻度
	float frequencyTime;               // 発生頻度用の現在の時刻

	ParticleValue<float> lifeTime;  // 寿命
	ParticleValue<float> moveSpeed; // 移動速度

	ParticleValue<Vector3> startScale;  // 開始スケール
	ParticleValue<Vector3> targetScale; // 目標スケール

	ParticleValue<Vector3> startRotationMultiplier;  // 開始回転倍率
	ParticleValue<Vector3> targetRotationMultiplier; // 目標回転倍率

	ParticleValue<Color> startColor;  // 開始色
	ParticleValue<Color> targetColor; // 目標色

	ParticleValue<Color> startVertexColor;  // 開始頂点色
	ParticleValue<Color> targetVertexColor; // 目標頂点色

	ParticleValue<float> startEmissiveIntensity;  // 開始発光強度
	ParticleValue<float> targetEmissiveIntensity; // 目標発光強度
	ParticleValue<Vector3> startEmissionColor;   // 開始発光色
	ParticleValue<Vector3> targetEmissionColor; // 目標発光色

	ParticleValue<float> startTextureAlphaReference;  // 貼るtexture、開始alphaReference
	ParticleValue<float> targetTextureAlphaReference; // 貼るtexture、目標alphaReference

	ParticleValue<float> startNoiseTextureAlphaReference;  // noiseTexture、開始alphaReference
	ParticleValue<float> targetNoiseTextureAlphaReference; // noiseTexture、目標alphaReference

	// texture
	uint32_t textureIndex;      // 貼るtexture
	uint32_t noiseTextureIndex; // noiseTexture

	// uv coming soon...

	// flags
	bool isLoop;          // ループするか
	bool useScaledTime;   // スケール時間を使用するか
	bool moveToDirection; // 進行方向に移動するか
	bool reflectGround;   // 地面に反射するか
	bool useNoiseTexture; // noiseTextureを使うか
	bool useVertexColor;  // 頂点色を使うか
	//--------- accessor -----------------------------------------------------

	const std::string& GetParticleName() const { return name_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemWidth_ = 200.0f;

	Asset* asset_;

	// particleの名前
	std::string name_;
	// modelの名前
	std::string modelName_;

	// textureの名前の保持
	std::string textureName_;
	std::string noiseTextureName_;

	//--------- functions ----------------------------------------------------

	void ImageButtonWithLabel(
		const char* id,           // ImGuiで重複しないID
		const std::string& label, // 表示したいテクスチャ名
		ImTextureID textureId,    // GPU ハンドル
		const ImVec2& size);      // 画像サイズ

	// 描画関係の値操作
	void EditRender();
	// 生成処理の値操作
	void EditEmit();
	// SRT関係の値操作
	void EditTransform();
	// 移動処理の値操作
	void EditMove();
	// material関係の値操作
	void EditMaterial();
};