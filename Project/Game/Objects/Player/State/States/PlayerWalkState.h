#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>

//============================================================================
//	PlayerWalkState class
//============================================================================
class PlayerWalkState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerWalkState() = default;
	~PlayerWalkState() = default;

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