#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Enemy/Boss/State/Interface/BossEnemyIState.h>

//============================================================================
//	BossEnemyIdleState class
//============================================================================
class BossEnemyIdleState :
	public BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyIdleState() = default;
	~BossEnemyIdleState() = default;

	void Enter(BossEnemy& bossEnemy) override;

	void Update(BossEnemy& bossEnemy) override;

	void Exit(BossEnemy& bossEnemy) override;

	// imgui
	void ImGui(const BossEnemy& bossEnemy) override;

	// json
	void ApplyJson([[maybe_unused]] const Json& data) override;
	void SaveJson([[maybe_unused]] Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// parameters
	float backStepSpeed_; // 後ずさりするときの速度

	//--------- functions ----------------------------------------------------

};