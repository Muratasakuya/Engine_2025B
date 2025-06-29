#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>

//============================================================================
//	BossEnemyTeleportationState class
//============================================================================
class BossEnemyTeleportationState :
	public BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyTeleportationState() = default;
	~BossEnemyTeleportationState() = default;

	void Enter(BossEnemy& bossEnemy) override;

	void Update(BossEnemy& bossEnemy) override;

	void Exit(BossEnemy& bossEnemy) override;

	// imgui
	void ImGui(const BossEnemy& bossEnemy) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	void SetTeleportType(BossEnemyTeleportType type) { type_ = type; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// テレポートの種類
	BossEnemyTeleportType type_;

	// parameters
	float farRadius_;  // 扇形半径(遠くに移動)
	float nearRadius_; // 扇形半径(近くに移動)
	float halfAngle_; // 扇形の半開き角

	// 座標
	Vector3 startPos_;  // 開始座標
	Vector3 targetPos_; // 目標座標

	float lerpTimer_;       // 座標補間の際の経過時間
	float lerpTime_;        // 座標補間にかける時間
	EasingType easingType_; // 補間の際のイージング
};