#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	PlayerStructures
//============================================================================

// 状態の種類
enum class PlayerState {

	None,          // 無状態、ここに状態は作成しない
	Idle,          // 何もしない
	Walk,          // 歩き...       WASD/左スティック入力
	Dash,          // ダッシュ...    右クリック/Aボタン
	Attack_1st,    // 通常攻撃1段目...左クリック/Xボタン
	Attack_2nd,    // 通常攻撃2段目...左クリック/Xボタン(1段目攻撃中にのみ入力受付)
	Attack_3rd,    // 通常攻撃3段目...左クリック/Xボタン(2段目攻撃中にのみ入力受付)
	SkilAttack,    // スキル攻撃...   E/Yボタン
	Parry,         // 攻撃カウンター...Space/ショルダーボタン
	SwitchAlly,    // 味方を切り替えるか入力(Idle or StunAttack)
	StunAttack,    // スタン攻撃、自動で行う
};

// ステータス
struct PlayerStats {

	int maxHP;     // 最大HP
	int currentHP; // 現在のHP

	int maxSkilPoint;     // 最大スキルポイント
	int currentSkilPoint; // 現在のスキルポイント

	std::unordered_map<PlayerState, int> damages; // 各攻撃のダメージ量
	int damageRandomRange;                        // ダメージのランダム範囲

	int toughness; // 攻撃した時に敵に入る靭性ダメージ量
};

// 遷移条件
struct PlayerStateCondition {

	float coolTime;                           // 次に入るまでのクールタイム
	std::vector<PlayerState> allowedPreState; // 遷移元を制限
	int requireSkillPoint;                    // 必要SP
	float chainInputTime;                     // 受付猶予
	std::vector<PlayerState> interruptableBy; // 強制キャンセルできる遷移相手

	void FromJson(const Json& data);
	void ToJson(Json& data);
};