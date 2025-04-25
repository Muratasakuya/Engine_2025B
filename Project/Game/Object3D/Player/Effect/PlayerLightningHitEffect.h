#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Base/BasePrimitiveEffect.h>

//============================================================================
//	PlayerLightningHitEffect class
//============================================================================
class PlayerLightningHitEffect :
	public BasePrimitiveEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerLightningHitEffect() = default;
	~PlayerLightningHitEffect() = default;

	void Init();

	void UpdateAnimation();

	// animation制御
	void StartAnimation(const Vector3& translate);
	void Reset();

	// imgui
	void ImGui();

	// json
	void SaveJson();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// animations
	// scale
	std::unique_ptr<SimpleAnimation<Vector3>> scaleAnimation_;
	// scroll
	std::unique_ptr<SimpleAnimation<float>> uvScrollAnimation_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson() override;

	// init
	void InitAnimation() override;
};