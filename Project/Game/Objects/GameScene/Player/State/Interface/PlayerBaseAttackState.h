#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Player/State/Interface/PlayerIState.h>
#include <Lib/Adapter/Easing.h>

//============================================================================
//	PlayerBaseAttackState class
//============================================================================
class PlayerBaseAttackState :
	public PlayerIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerBaseAttackState() = default;
	~PlayerBaseAttackState() = default;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	float exitTimer_; // 経過時間
	float exitTime_;  // 次に状態に遷移できるまでの時間

	float attackPosLerpCircleRange_; // 攻撃したとき指定の座標まで補間する範囲(円の半径)
	float attackLookAtCircleRange_;  // 攻撃したとき敵の方向を向く範囲(円の半径)
	float attackOffsetTranslation_;  // 敵の座標からのオフセット距離
	float attackPosLerpTimer_;       // 座標補間の際の経過時間
	float attackPosLerpTime_;        // 座標補間にかける時間
	EasingType attackPosEaseType_;   // 座標補間に使用するイージングの種類

	// 補間目標
	std::optional<Vector3> targetTranslation_;
	std::optional<Quaternion> targetRotation_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson(const Json& data);
	void SaveJson(Json& data);

	// imgui
	void ImGui(const Player& player);

	void ResetTarget();

	// update
	void AttackAssist(Player& player, bool onceTarget = false);

	// helper
	Vector3 GetPlayerOffsetPos(const Player& player, const Vector3& offsetTranslation) const;
	Matrix4x4 GetPlayerOffsetRotation(const Player& player, const Vector3& offsetRotation) const;

	// debug
	void DrawAttackOffset(const Player& player);
};