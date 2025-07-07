#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>
#include <Game/Objects/Player/State/Interface/PlayerIState.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	PlayerSwitchAllyState class
//============================================================================
class PlayerSwitchAllyState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerSwitchAllyState();
	~PlayerSwitchAllyState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool GetIsSwitched();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	float deltaTimeScaleTimer_; // スローモーションにするまでの経過時間
	float deltaTimeScaleTime_;  // スローモーションにするまでの時間
	EasingType deltaTimeScaleEasingType_;
	float deltaTimeScale_;      // スローモーション用

	float switchAllyTimer_; // 現在の経過時間
	float switchAllyTime_;  // 切り替え選択の行える時間

	// postProcess
	float blurTimer_; // ブラーの経過時間
	float blurTime_;  // ブラーの時間
	EasingType blurEasingType_;
	RadialBlurForGPU radialBlur_;       // systemに渡す値
	RadialBlurForGPU targetRadialBlur_; // 補間先の値

	// 切り替えたかどうかのフラグ
	std::optional<bool> isSwitched_;

	//--------- functions----------------------------------------------------

	// update
	void UpdateBlur();
	void CheckInput(float t);
};