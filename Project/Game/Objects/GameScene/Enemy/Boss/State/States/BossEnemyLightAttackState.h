#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Enemy/Boss/State/Interface/BossEnemyIState.h>

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

	//--------- stricture ----------------------------------------------------

	// 状態
	enum class State {

		ParrySign, // パリィ受付中
		Attack     // 攻撃
	};

	//--------- variables ----------------------------------------------------

	// 状態
	State currentState_;

	// 座標
	Vector3 startPos_; // 開始座標

	// parameters
	float lerpTimer_;       // 座標補間の際の経過時間
	EasingType easingType_; // 補間の際のイージング

	float attackOffsetTranslation_; // 座標からのオフセット距離

	float exitTimer_; // 遷移可能にするまでの経過時間
	float exitTime_;  // 遷移可能にするまでの時間

	// debug
	bool parried_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdateParrySign(BossEnemy& bossEnemy);
	void UpdateAttack(BossEnemy& bossEnemy);
	void UpdateParryTiming(BossEnemy& bossEnemy);
};