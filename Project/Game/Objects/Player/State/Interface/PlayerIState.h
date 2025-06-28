#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/Easing.h>
#include <Lib/MathUtils/MathUtils.h>

// front
class Player;
class PlayerInputMapper;
class BossEnemy;
class FollowCamera;

//============================================================================
//	PlayerIState class
//============================================================================
class PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerIState() = default;
	virtual ~PlayerIState() = default;

	// 状態遷移時
	virtual void Enter(Player& player) = 0;

	// 更新処理
	virtual void Update(Player& player) = 0;

	// 状態終了時
	virtual void Exit(Player& player) = 0;

	// imgui
	virtual void ImGui(const Player& player) = 0;

	// json
	virtual void ApplyJson([[maybe_unused]] const Json& data) = 0;
	virtual void SaveJson([[maybe_unused]] Json& data) = 0;

	//--------- accessor -----------------------------------------------------

	void SetInputMapper(const PlayerInputMapper* inputMapper) { inputMapper_ = inputMapper; }
	void SetBossEnemy(const BossEnemy* bossEnemy) { bossEnemy_ = bossEnemy; }
	void SetFollowCamera(const FollowCamera* followCamera) { followCamera_ = followCamera; }

	virtual bool GetCanExit() const { return true; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const PlayerInputMapper* inputMapper_;
	const BossEnemy* bossEnemy_;
	const FollowCamera* followCamera_;

	// 共通parameters
	const float epsilon_ = std::numeric_limits<float>::epsilon();

	float nextAnimDuration_; // 次のアニメーション遷移にかかる時間
	bool animationFinished_; // アニメーションが終了したかどうか

	float rotationLerpRate_; // 回転補間割合

	// 攻撃共通parameters
	float attackPosLerpCircleRange_; // 攻撃したとき指定の座標まで補間する範囲(円の半径)
	float attackLookAtCircleRange_;  // 攻撃したとき敵の方向を向く範囲(円の半径)
	float attackOffsetTranslation_;  // 敵の座標からのオフセット距離
	float attackPosLerpTimer_;       // 座標補間の際の経過時間
	float attackPosLerpTime_;        // 座標補間にかける時間
	EasingType attackPosEaseType_;   // 座標補間に使用するイージングの種類

	//--------- functions ----------------------------------------------------

	// helper
	void SetRotateToDirection(Player& player, const Vector3& move);
	void AttackAssist(Player& player);

	// debug
	void DrawAttackOffset(const Player& player);
	void DrawAttackRangeCircle(const Player& player, float range);
};