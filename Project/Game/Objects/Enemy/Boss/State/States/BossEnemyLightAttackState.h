#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>

//============================================================================
//	BossEnemyLightAttackState class
//============================================================================
class BossEnemyLightAttackState :
	public BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyLightAttackState() = default;
	~BossEnemyLightAttackState() = default;

	void Enter(BossEnemy& bossEnemy) override;

	void Update(BossEnemy& bossEnemy) override;

	void Exit(BossEnemy& bossEnemy) override;

	// imgui
	void ImGui(const BossEnemy& bossEnemy) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------
	
	// animation制御に使用
	bool playAnimation_;

	// 座標
	Vector3 startPos_;  // 開始座標
	Vector3 targetPos_; // 目標座標

	// parameters
	float lerpTimer_;       // 座標補間の際の経過時間
	float lerpTime_;        // 座標補間にかける時間
	EasingType easingType_; // 補間の際のイージング

	float attackOffsetTranslation_;  // 座標からのオフセット距離

	float exitTimer_; // 遷移可能にするまでの経過時間
	float exitTime_;  // 遷移可能にするまでの時間
};