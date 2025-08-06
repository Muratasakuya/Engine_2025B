#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Player/State/Interface/PlayerBaseAttackState.h>
#include <Engine/Effect/GameEffect.h>

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

	PlayerAttack_2ndState();
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

	static constexpr size_t kNumSegments = 3;

	// 敵との距離に応じた処理を行うフラグ
	bool approachPhase_;

	Vector3 startTranslation_;                    // 移動開始地点
	std::array<Vector3, kNumSegments> wayPoints_; // 移動する点の数
	size_t currentIndex_; // 現在の区間
	float segmentTimer_;  // 区間経過時間
	float segmentTime_;   //区間にかかる時間
	float swayRate_;      //揺れ幅

	float leftPointAngle_;  // 左の座標の角度
	float rightPointAngle_; // 右の座標の角度

	// parameters
	// 斬撃エフェクト
	std::array<Vector3, kNumSegments> slashEffectRotations_;   // 回転
	std::array<Vector3, kNumSegments> slashEffectTranslatons_; // 座標
	bool emittedThisSegment_;

	// 斬撃
	std::unique_ptr<GameEffect> slashEffect_;

	// debug
	std::array<Vector3, 3> debugWayPoints_;

	//--------- functions ----------------------------------------------------

	void CalcWayPoints(const Player& player, std::array<Vector3, 3>& dstWayPoints);

	void EmitSlashEffectForCurrentIndex(Player& player);
};