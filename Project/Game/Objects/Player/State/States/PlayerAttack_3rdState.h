#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerBaseAttackState.h>
#include <Engine/Effect/GameEffect.h>

//============================================================================
//	PlayerAttack_3rdState class
//============================================================================
class PlayerAttack_3rdState :
	public PlayerBaseAttackState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttack_3rdState();
	~PlayerAttack_3rdState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	bool emitEffect_; // effectを発生させたか

	// effectParameters
	float groundEffectDistance_; // plyerからの距離
	float groundEffectPosY_;     // Y座標

	// 地割れ
	std::unique_ptr<GameEffect> groungEffect_;
};