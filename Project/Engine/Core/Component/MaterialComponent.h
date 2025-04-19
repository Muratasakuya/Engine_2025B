#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/CBufferStructures.h>
#include <Engine/Utility/SimpleAnimation.h>
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
	void SetUVScrollValue(float value);

	//--------- variables ----------------------------------------------------

	EffectMaterial material;

	UVTransform uvTransform;

	//--------- firend -------------------------------------------------------

	friend void swap(EffectMaterialComponent& a, EffectMaterialComponent& b) noexcept;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	SimpleAnimation<float> uvScrollAnimation_;

	UVTransform prevUVTransform_;

	//--------- functions ----------------------------------------------------

	void UpdateUVScroll();

	void EditMaterial(float itemSize);
};