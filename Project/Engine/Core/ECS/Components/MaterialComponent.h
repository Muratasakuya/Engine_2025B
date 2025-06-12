#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Utility/SimpleAnimation.h>
#include <Lib/MathUtils/MathUtils.h>

// front
class Asset;

//============================================================================
//	UVTransform
//============================================================================

struct UVTransform {

	Vector3 scale;
	Vector3 rotate;
	Vector3 translation;

	// operator
	bool operator==(const UVTransform& other) const {

		return scale == other.scale &&
			rotate == other.rotate &&
			translation == other.translation;
	}
};

//============================================================================
//	MaterialComponent class
//============================================================================
class MaterialComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MaterialComponent() = default;
	~MaterialComponent() = default;

	void Init(Asset* asset);

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	void SetTextureName(const std::string& textureName);

	//--------- variables ----------------------------------------------------

	Color color; // 色

	// テクスチャインデックス
	uint32_t textureIndex;
	uint32_t normalMapTextureIndex;

	// 法線マップ
	int32_t enableNormalMap;

	// ライティング
	int32_t enableLighting;
	int32_t enableHalfLambert;
	int32_t enableBlinnPhongReflection;
	
	float phongRefShininess;
	Vector3 specularColor;

	// 影倍率(0.0fで真っ黒な影、1.0fで影無し)
	float shadowRate;

	// 環境マップ
	int32_t enableImageBasedLighting;
	float environmentCoefficient;

	// 発光度
	float emissiveIntensity;
	Vector3 emissionColor;

	// uv
	Matrix4x4 uvMatrix;
	UVTransform uvTransform;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	UVTransform prevUVTransform_;

	//--------- functions ----------------------------------------------------

	// init
	void InitParameter();
};

//============================================================================
//	SpriteMaterialComponent class
//============================================================================
class SpriteMaterialComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpriteMaterialComponent() = default;
	~SpriteMaterialComponent() = default;

	void Init(ID3D12Device* device);

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- variables ----------------------------------------------------

	SpriteMaterial material;

	UVTransform uvTransform;

	//--------- accessor -----------------------------------------------------

	const DxConstBuffer<SpriteMaterial>& GetBuffer() const { return buffer_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	UVTransform prevUVTransform_;

	// buffer
	DxConstBuffer<SpriteMaterial> buffer_;
};