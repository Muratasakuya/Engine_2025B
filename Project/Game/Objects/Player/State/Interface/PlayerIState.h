#pragma once

//============================================================================
//	include
//============================================================================
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
	virtual void ImGui() = 0;

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

	//--------- functions ----------------------------------------------------

	// helper
	void SetRotateToDirection(Player& player, const Vector3& move);
};