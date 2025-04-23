#pragma once

//============================================================================
//	PlayerBehaviorType enum class
//============================================================================

// behaviorの種類
enum class PlayerBehaviorType {

	// 移動
	Dash,

	// 攻撃
	NormalAttack, // 歩きながら攻撃
	DashAttack,   // 走りながら攻撃
};