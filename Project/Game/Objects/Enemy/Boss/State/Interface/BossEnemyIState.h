#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <string>
// front
class BossEnemy;
class Player;

//============================================================================
//	BossEnemyIState class
//============================================================================
class BossEnemyIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyIState() = default;
	virtual ~BossEnemyIState() = default;

	// 状態遷移時
	virtual void Enter(BossEnemy& bossEnemy) = 0;

	// 更新処理
	virtual void Update(BossEnemy& bossEnemy) = 0;

	// 状態終了時
	virtual void Exit(BossEnemy& bossEnemy) = 0;

	// imgui
	virtual void ImGui() {}

	// json
	virtual void ApplyJson([[maybe_unused]] const Json& data) = 0;
	virtual void SaveJson([[maybe_unused]] Json& data) = 0;

	//--------- accessor -----------------------------------------------------

	void SetPlayer(const Player* player) { player_ = player; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const Player* player_;

	// 共通parameters
	float nextAnimDuration_; // 次のアニメーション遷移にかかる時間
	float rotationLerpRate_; // 回転補完割合

};