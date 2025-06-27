#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerDashState class
//============================================================================
class PlayerDashState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerDashState() = default;
	~PlayerDashState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui() override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};