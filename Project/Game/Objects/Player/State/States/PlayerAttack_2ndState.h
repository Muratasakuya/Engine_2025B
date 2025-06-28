#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerBaseAttackState.h>

//============================================================================
//	PlayerAttack_2ndState class
//============================================================================
class PlayerAttack_2ndState :
	public PlayerBaseAttackState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttack_2ndState() = default;
	~PlayerAttack_2ndState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;
};