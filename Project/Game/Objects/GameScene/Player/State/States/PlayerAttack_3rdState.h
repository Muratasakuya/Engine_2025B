#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Player/State/Interface/PlayerBaseAttackState.h>

//============================================================================
//	PlayerAttack_3rdState class
//============================================================================
class PlayerAttack_3rdState :
	public PlayerBaseAttackState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttack_3rdState();
	~PlayerAttack_3rdState() = default;

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

		None,     // 何もなし
		MoveBack, // 後ずさり
		Catch     // 剣を取りに行く
	};

	// 武器のパラメータ
	struct WeaponParam {

		bool isMoveStart;      // 移動開始フラグ
		StateTimer moveTimer;  // 共通から値を受け取る
		float moveValue;       // 移動量
		float offsetRotationY; // 進行方向のオフセットY回転
		Vector3 startPos;  // 開始座標(親子付けを外した瞬間のワールド座標)
		Vector3 targetPos; // 目標座標
	};

	//--------- variables ----------------------------------------------------

	// プレイヤーの現在の状態
	State currentState_;
	float initPosY_;

	// parameters
	// プレイヤー
	StateTimer backMoveTimer_; // 後ずさりする時間
	float backMoveValue_;      // 後ずさりする距離
	Vector3 backStartPos_;     // 後ずさり開始座標
	Vector3 backTargetPos_;    // 後ずさり目標座標

	StateTimer catchSwordTimer_; // 飛ばした剣の場所まで行く時間
	Vector3 catchTargetPos_;     // 剣の目標座標(2つの剣の間)

	// 剣
	std::unordered_map<PlayerWeaponType, WeaponParam> weaponParams_;
	// 共通
	StateTimer weaponMoveTimer_; // 剣が目標座標に行くまでの時間

	//--------- functions ----------------------------------------------------

	// update
	void UpdateAnimKeyEvent(Player& player);
	void LerpWeapon(Player& player, PlayerWeaponType type);
	void LerpPlayer(Player& player);

	// helper
	void StartMoveWeapon(Player& player, PlayerWeaponType type);
};