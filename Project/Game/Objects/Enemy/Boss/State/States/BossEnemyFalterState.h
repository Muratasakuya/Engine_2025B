#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>

//============================================================================
//	BossEnemyFalterState class
//============================================================================
class BossEnemyFalterState :
	public BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyFalterState() = default;
	~BossEnemyFalterState() = default;

	void Enter(BossEnemy& bossEnemy) override;

	void Update(BossEnemy& bossEnemy) override;

	void Exit(BossEnemy& bossEnemy) override;

	// imgui
	void ImGui() override;

	// json
	void ApplyJson([[maybe_unused]] const Json& data) override;
	void SaveJson([[maybe_unused]] Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// parameters
	float backStepSpeed_;    // 後ずさりするときの速度

	//--------- functions ----------------------------------------------------

};