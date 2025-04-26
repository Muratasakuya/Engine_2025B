#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Utility/SimpleAnimation.h>

//============================================================================
//	BasePrimitiveEffect class
//============================================================================
class BasePrimitiveEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BasePrimitiveEffect() = default;
	virtual ~BasePrimitiveEffect() = default;

	void Init(const std::string& modelName, const std::string& textureName,
		const std::string& objectName, const std::string& groupName);

	//--------- accessor -----------------------------------------------------

protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// imgui
	const float itemWidth_ = 224.0f;

	// components
	EffectTransformComponent* transform_;
	EffectMaterialComponent* material_;
	PrimitiveMeshComponent* mesh_;

	//--------- functions ----------------------------------------------------

	// json
	virtual void ApplyJson() {}
	void ApplyComponent(const Json& data);
	void SaveComponent(Json& data);

	// imgui
	void EditComponent();

	// init
	virtual void InitAnimation() {}

	// helper
	// 描画範囲外に移す
	void OutsideEffect();
};