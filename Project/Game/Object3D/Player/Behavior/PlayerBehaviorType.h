#pragma once

//============================================================================
//	PlayerBehaviorType enum class
//============================================================================

// behaviorの種類
enum class PlayerBehaviorType {

	// 移動
	Dash,

	// 攻撃1段目
	Attack_1st, // 止まっている状態から攻撃...1段目
	DashAttack, // ダッシュ攻撃...1段目

	// 攻撃2段目以降、1段目から繋げる
	// Attack_1st、DashAttackどちらからでも
	// こっちに繋がる
	Attack_2nd,
	Attack_3rd,

	// 攻撃受け流し
	Parry,
};