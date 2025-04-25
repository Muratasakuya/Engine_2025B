#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Utility/SimpleAnimation.h>

//============================================================================
//	PlayerLightningHitEffect class
//============================================================================
class PlayerLightningHitEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerLightningHitEffect() = default;
	~PlayerLightningHitEffect() = default;

	void Init();

	void UpdateAnimation();

	void ImGui();

	void Reset();

	// json
	void SaveJson();

	//--------- accessor -----------------------------------------------------

	void StartAnimation();

	void SetParent(const BaseTransform& parent);
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
	// scroll
	std::unique_ptr<SimpleAnimation<float>> uvScrollAnimation_;
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