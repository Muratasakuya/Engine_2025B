#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerAttack_1stState class
//============================================================================
class PlayerAttack_1stState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttack_1stState() = default;
	~PlayerAttack_1stState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson([[maybe_unused]] const Json& data) override;
	void SaveJson([[maybe_unused]] Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override { return animationFinished_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};