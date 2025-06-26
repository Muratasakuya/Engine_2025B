#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>
#include <Game/Objects/Enemy/Boss/Structures/BossEnemyStructures.h>

// c++
#include <memory>
#include <optional>
#include <unordered_map>
// front
class Player;

//============================================================================
//	BossEnemyStateController class
//============================================================================
class BossEnemyStateController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyStateController() = default;
	~BossEnemyStateController() = default;

	void Init(BossEnemy& owner);

	void Update(BossEnemy& owner);

	// 状態遷移をリクエスト
	void RequestState(BossEnemyState state) { requested_ = state; }

	void ImGui();
	//--------- accessor -----------------------------------------------------

	void SetPlayer(const Player* player);

	void SetStatas(const BossEnemyStats& stats) { stats_ = stats; }

	BossEnemyState GetCurrentState() const { return current_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// ステータス
	BossEnemyStats stats_;

	// 現在のフェーズ
	uint32_t currentPhase_;

	std::unordered_map<BossEnemyState, std::unique_ptr<BossEnemyIState>> states_;

	BossEnemyState current_;                  // 現在の状態
	std::optional<BossEnemyState> requested_; // 次の状態

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdatePhase();

	// helper
	void ChangeState(BossEnemy& owner);
};