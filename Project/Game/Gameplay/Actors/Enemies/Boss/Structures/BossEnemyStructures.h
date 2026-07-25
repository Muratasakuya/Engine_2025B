#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/MathLib/MathUtils.h>
#include <Engine/Utility/Timer/StateTimer.h>
#include <Engine/Utility/Enum/Distance.h>

// c++
#include <cstdint>
#include <vector>

//============================================================================
//	BossEnemyStructures
//	ボス敵の状態遷移、フェーズ、コンボ、パリィ条件で共有するデータ構造群。
//============================================================================

// 状態の種類
enum class BossEnemyState {

	Idle,             // 何もしない
	Teleport,         // 瞬間移動(実際には高速で補間する)
	Stun,             // スタン状態
	Falter,           // 怯む
	LightAttack,      // 弱攻撃
	StrongAttack,     // 強攻撃
	ChargeAttack,     // 溜め攻撃
	RushAttack,       // 突進攻撃
	ContinuousAttack, // 連続攻撃
	GreatAttack,      // 大技攻撃
	JumpAttack,       // ジャンプ攻撃
	ProjectileAttack, // 飛び道具攻撃
	AllFieldAttack,   // 全方位攻撃
	Count,
};

// テレポートの種類
enum class BossEnemyTeleportType {

	Far, // 遠くに
	Near // 近くに
};

/// <summary>
/// HP、靭性、攻撃ダメージ、距離レベルなどボス戦全体で参照するステータスを保持する構造体。
/// </summary>
struct BossEnemyStats {

	int maxHP;     // 最大HP
	int currentHP; // 現在のHP

	int maxDestroyToughness;     // 撃破靭性値
	int currentDestroyToughness; // 現在の撃破靭性

	// 閾値リストの条件
	// indexNはindexN+1の値より必ず大きい(N=80、N+1=85にはならない)
	std::vector<int> hpThresholds; // HP割合の閾値リスト

	std::unordered_map<BossEnemyState, int> damages; // 各攻撃のダメージ量
	int damageRandomRange;                           // ダメージのランダム範囲

	// 距離レベル判定距離
	std::unordered_map<DistanceLevel, float> distanceLevels;
	// プレイヤーまでの現在距離
	float currentDistanceToTarget;
	// プレイヤーまでの距離レベル
	DistanceLevel currentDistanceLevel;
};

/// <summary>
/// ボスが実行する状態列と、発動可能距離、繰り返し可否、テレポート種別をまとめる構造体。
/// </summary>
struct BossEnemyCombo {

	std::vector<BossEnemyState> sequence; // コンボの順序
	bool allowRepeat;                     // 同じComboを繰り返してもよいか
	BossEnemyTeleportType teleportType;   // テレポートの種類

	// コンボを発動できる距離レベル、何も値が入っていなければどこでも処理できるようにする
	std::vector<DistanceLevel> requiredDistanceLevels;

	void FromJson(const Json& data);
	void ToJson(Json& data);
};

/// <summary>
/// HP閾値で切り替わる各フェーズの遷移時間、使用コンボ、攻撃後の待機復帰を定義する構造体。
/// </summary>
struct BossEnemyPhase {

	float nextStateDuration = 1.0f; // この秒数経過で次状態へ遷移
	std::vector<int> comboIndices;  // コンボインデックスのリスト
	bool autoIdleAfterAttack;       // 強制的に待機状態に戻すか

	void FromJson(const Json& data);
	void ToJson(Json& data);
};

/// <summary>
/// jsonから読み書きするボスAI用のコンボ一覧とフェーズ一覧を保持する構造体。
/// </summary>
struct BossEnemyStateTable {
	
	std::vector<BossEnemyCombo> combos;
	std::vector<BossEnemyPhase> phases;

	void FromJson(const Json& data);
	void ToJson(Json& data);
};

/// <summary>
/// パリィ受付可否と必要回数を保持する。連続パリィは全回数を受けた後に反撃可能になる。
/// </summary>
struct ParryParameter {

	bool canParry = false;        // パリィ可能かどうか
	uint32_t continuousCount = 0; // パリィ回数(連続パリィ)
};
