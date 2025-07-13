#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	PlayerSkilAttackState class
//============================================================================
class PlayerSkilAttackState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerSkilAttackState() = default;
	~PlayerSkilAttackState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 状態
	enum class State {

		Approach, // 近づく
		Leave,    // 離れる
	};

	// 補間
	struct LerpParameter {

		float timer;
		float time;
		EasingType easingType;

		Vector3 Update(const Vector3& start, const Vector3& target);
		void ImGui(const std::string& label);
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State currentState_;

	// parameters
	LerpParameter approach_; // 近づくとき
	LerpParameter leave_;    // 離れるとき
	float moveDistance_;     // 移動距離

	Vector3 startPos_;  // 開始座標
	Vector3 targetPos_; // 目標座標

	//--------- functions ----------------------------------------------------

	void UpdateState(Player& player);
};