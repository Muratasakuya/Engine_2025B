#pragma once

//============================================================================
//	include
//============================================================================

//============================================================================
//	PlayerStructures class
//============================================================================

// 状態の種類
enum class PlayerState {

	Idle,          // 何もしない
	Walk,          // 歩き
	Dash,          // ダッシュ
	Attack_1st,    // 通常攻撃1段目
	Attack_2nd,    // 通常攻撃2段目
	Attack_3rd,    // 通常攻撃3段目
	DashAttack,    // ダッシュ攻撃
	SkilAttack,    // スキル攻撃
	SpecialAttack, // 必殺
	Parry,         // 攻撃カウンター(実装優先度低め)
};

// ステータス
struct PlayerStats {

	int maxHP;     // 最大HP
	int currentHP; // 現在のHP

	int maxSkilPoint;     // 最大スキルポイント
	int currentSkilPoint; // 現在のスキルポイント
};