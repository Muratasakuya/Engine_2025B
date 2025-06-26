#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <vector>

//============================================================================
//	BossEnemyStructures class
//============================================================================

// memo
// HP残量に応じて攻撃パターンを変える、数が変更する可能性あり
// 100% ~ 80%... 弱攻撃のみ
// 80% ~ 50%...  弱攻撃の連撃、溜め攻撃
// 50% ~ 0%...   全ての攻撃パターンを使用する

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

	// 閾値リストの条件
	// indexNはindexN+1の値より必ず大きい(N=80、N+1=85にはならない)
	std::vector<int> hpThresholds; // HP割合の閾値リスト

	int maxDestroyToughness;     // 撃破靭性値
	int currentDestroyToughness; // 現在の撃破靭性値
};