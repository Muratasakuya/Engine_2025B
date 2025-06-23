#pragma once

//============================================================================
//	include
//============================================================================

//============================================================================
//	BossEnemyStructures class
//============================================================================

// 状態の種類
enum class BossEnemyState {

	Idle,         // 何もしない
	Falter,       // 怯む
	LightAttack,  // 弱攻撃
	StrongAttack, // 強攻撃
};

// ステータス
struct BossEnemyStats {

	int maxHP;     // 最大HP
	int currentHP; // 現在のHP

	int maxDestroyToughness;     // 撃破靭性値
	int currentDestroyToughness; // 現在の撃破靭性値
};