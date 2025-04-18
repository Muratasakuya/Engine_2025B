#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	UVTransform
//============================================================================

struct UVTransform {

	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

	// operator
	bool operator==(const UVTransform& other) const {

		return scale == other.scale &&
			rotate == other.rotate &&
			translate == other.translate;
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

	void Init();

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- variables ----------------------------------------------------

	Material material;

	UVTransform uvTransform;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	UVTransform prevUVTransform;
};

//============================================================================
//	EffectMaterialComponent class
//============================================================================
class EffectMaterialComponent {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	EffectMaterialComponent() = default;
	~EffectMaterialComponent() = default;

	void Init();

	void UpdateUVTransform();

	void ImGui(float itemSize);

	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	// 横scrollのみ、縦は未実装
	void SetUVScrollValue(float value) { addUVTranslateValue_.x = value; }

	//--------- variables ----------------------------------------------------

	EffectMaterial material;

	UVTransform uvTransform;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	UVTransform prevUVTransform;

	Vector2 addUVTranslateValue_;
	bool uvScrollEnable_;

	bool roopUVScrollEnable_; // ループするかどうか
	float totalScrollValue_;  // スクロール量の合計値

	// UVTransformを操作するための変数
	float uvScrollElapsedTime_; // 経過時間
	float uvScrollWaitTime_;    // 待ち時間

	//--------- functions ----------------------------------------------------

	void UpdateUVScroll();
};