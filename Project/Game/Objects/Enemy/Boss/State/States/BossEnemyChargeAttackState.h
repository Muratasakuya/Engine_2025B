#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>

//============================================================================
//	BossEnemyChargeAttackState class
//============================================================================
class BossEnemyChargeAttackState :
	public BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyChargeAttackState() = default;
	~BossEnemyChargeAttackState() = default;

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

	// parameters
	float exitTimer_; // 遷移可能にするまでの経過時間
	float exitTime_;  // 遷移可能にするまでの時間
};