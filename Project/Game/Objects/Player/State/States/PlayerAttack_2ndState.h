#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerBaseAttackState.h>

// c++
#include <array>

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

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 敵との距離に応じた処理を行うフラグ
	bool approachPhase_;

	Vector3 startTranslation_;         // 移動開始地点
	std::array<Vector3, 3> wayPoints_; // 移動する点の数
	size_t currentIndex_; // 現在の区間
	float segmentTimer_;  // 区間経過時間
	float segmentTime_;   //区間にかかる時間
	float swayRate_;      //揺れ幅

	float leftPointAngle_;  // 左の座標の角度
	float rightPointAngle_; // 右の座標の角度

	// debug
	std::array<Vector3, 3> debugWayPoints_;

	//--------- functions ----------------------------------------------------

	void CalcWayPoints(const Player& player, std::array<Vector3, 3>& dstWayPoints);
};