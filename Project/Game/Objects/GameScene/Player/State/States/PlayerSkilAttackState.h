#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Player/State/Interface/PlayerBaseAttackState.h>

//============================================================================
//	PlayerSkilAttackState class
//============================================================================
class PlayerSkilAttackState :
	public PlayerBaseAttackState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerSkilAttackState();
	~PlayerSkilAttackState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 状態
	enum class State {

		Rush,      // 突進
		Return,    // 戻る
		JumpAttack // 戻った反動でジャンプして敵に攻撃
	};
	enum class RotateState {

		RotateX,
		LookEnemy
	};

	// 各状態
	struct StateMoveParam {

		StateTimer timer; // 状態処理時間
		float moveValue;  // 移動量
		Vector3 start;    // 開始
		Vector3 target;   // 目標
		float nextAnim;   // 次のアニメーションまでの時間
		std::string name; // 名前

		void ImGui(const Player& player, const BossEnemy& bossEnemy);
		void ApplyJson(const Json& data);
		void SaveJson(Json& data);
	};

	// ジャンプ
	struct JumpParam {

		float power;   // 力
		float gravity; // 重力
		std::string name; // 名前

		void ImGui();
		void ApplyJson(const Json& data);
		void SaveJson(Json& data);
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State currentState_;
	RotateState rotateState_;
	// 範囲内にいるか
	bool assisted_;
	// 現在のジャンプ力Y
	float velocityY_;

	// parameters
	// Rush
	StateMoveParam rushMoveParam_;
	// Return
	StateMoveParam returnMoveParam_;
	// 回転
	StateTimer rotateXTimer_;   // 1回転する時間
	StateTimer lookEnemyTimer_; // 敵の方向を向くまでの時間
	Quaternion stratRotation;   // 初期回転
	Quaternion yawLerpStart_; // 補間開始
	Quaternion yawLerpEnd_;   // 補間目標
	// ジャンプ
	JumpParam returnJumpParam_; // 戻るときのジャンプ
	// JumpAttack
	StateMoveParam jumpAttackMoveParam_;

	// エディター
	State editState_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdateState(Player& player);
	void UpdateRush(Player& player);
	void UpdateReturn(Player& player);
	void UpdateJumpAttack(Player& player);
};