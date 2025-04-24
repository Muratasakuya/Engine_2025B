#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Utility/SimpleAnimation.h>

//============================================================================
//	PlayerCircleHitEffect class
//============================================================================
class PlayerCircleHitEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerCircleHitEffect() = default;
	~PlayerCircleHitEffect() = default;

	void Init();

	void UpdateAnimation();

	void ImGui();

	// json
	void SaveJson();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	EffectTransformComponent* transform_;
	EffectMaterialComponent* material_;
	PrimitiveMeshComponent* mesh_;

	// parameter
	// scale
	std::unique_ptr<SimpleAnimation<Vector3>> scaleAnimation_;
	// alpha
	std::unique_ptr<SimpleAnimation<float>> alphaAnimation_;

	// emitEditor
	bool emitEffect_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();

	// init
	void InitComponent();
	void InitAnimation();
};