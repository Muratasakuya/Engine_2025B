#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Structures/PlayerStructures.h>
#include <Lib/MathUtils/MathUtils.h>

// front
class Player;
class PlayerInputMapper;
class BossEnemy;
class FollowCamera;
class PostProcessSystem;

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
	virtual void ApplyJson(const Json& data) = 0;
	virtual void SaveJson(Json& data) = 0;

	//--------- accessor -----------------------------------------------------

	void SetInputMapper(const PlayerInputMapper* inputMapper) { inputMapper_ = inputMapper; }
	void SetBossEnemy(const BossEnemy* bossEnemy) { bossEnemy_ = bossEnemy; }
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	void SetCanExit(bool canExit) { canExit_ = canExit; }
	void SetPreState(PlayerState preState) { preState_ = preState; }
	void SetMoveClampSize(float size) { moveClampSize_ = size; }

	virtual bool GetCanExit() const { return canExit_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const PlayerInputMapper* inputMapper_;
	const BossEnemy* bossEnemy_;
	FollowCamera* followCamera_;

	// 遷移前の状態
	PlayerState preState_;

	// 共通parameters
	const float epsilon_ = std::numeric_limits<float>::epsilon();

	float nextAnimDuration_; // 次のアニメーション遷移にかかる時間
	bool canExit_ = true;    // 遷移可能かどうか
	float rotationLerpRate_; // 回転補間割合
	float moveClampSize_;    // 移動できる範囲の制限

	//--------- functions ----------------------------------------------------

	// helper
	void SetRotateToDirection(Player& player, const Vector3& move);
};