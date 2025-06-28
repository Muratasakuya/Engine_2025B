#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/SimpleAnimation.h>
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

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 move_;    // 移動量
	float moveSpeed_; // 移動速度

	// ダッシュの速度補間
	std::unique_ptr<SimpleAnimation<float>> speedLerpValue_;

	//--------- functions ----------------------------------------------------

	void UpdateDash(Player& player);
};