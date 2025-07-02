#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Enemy/Boss/State/Interface/BossEnemyIState.h>
#include <Game/Objects/Enemy/Boss/Structures/BossEnemyStructures.h>

// c++
#include <memory>
#include <optional>
#include <functional>
#include <unordered_map>
// imgui
#include <imgui.h>

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

	void ImGui(const BossEnemy& bossEnemy);
	void EditStateTable();

	//--------- accessor -----------------------------------------------------

	void SetPlayer(const Player* player);
	void SetFollowCamera(const FollowCamera* followCamera);

	void SetStatas(const BossEnemyStats& stats) { stats_ = stats; }

	BossEnemyState GetCurrentState() const { return current_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// ステータス
	BossEnemyStats stats_;
	// 状態デーブル
	BossEnemyStateTable stateTable_;
	// 再生中情報
	uint32_t currentSequenceIndex_;
	uint32_t currentComboSlot_;
	uint32_t currentComboIndex_;
	uint32_t prevPhase_;
	uint32_t prevComboIndex_;
	float stateTimer_;

	// 現在のフェーズ
	uint32_t currentPhase_;

	std::unordered_map<BossEnemyState, std::unique_ptr<BossEnemyIState>> states_;

	BossEnemyState current_;                    // 現在の状態
	std::optional<BossEnemyState> requested_;   // 次の状態
	std::optional<BossEnemyState> forcedState_; // 状態の強制遷移

	// editor
	int editingStateIndex_;
	const ImVec4 kHighlight = ImVec4(1.0f, 0.85f, 0.2f, 1.0f);
	bool disableTransitions_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdatePhase();
	void UpdateStateTimer();

	// helper
	void ChangeState(BossEnemy& owner);
	void ChooseNextState(const BossEnemyPhase& phase);
	void CheckStunToughness();
	void SyncPhaseCount();
	void DrawHighlighted(bool highlight, const ImVec4& col, const std::function<void()>& draw);
};