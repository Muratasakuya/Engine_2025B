#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Utility/SimpleAnimation.h>

//============================================================================
//	PlayerBurstHitEffect class
//============================================================================
class PlayerBurstHitEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerBurstHitEffect() = default;
	~PlayerBurstHitEffect() = default;

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
	std::unique_ptr<SimpleAnimation<float>> uvScrollAnimation_;

	// emitEditor
	bool emitEffect_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();

	// init
	void InitComponent();
	void InitAnimation();
};