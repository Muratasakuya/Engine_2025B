#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/State/Interface/PlayerIState.h>
#include <Game/Objects/Player/Input/PlayerInputMapper.h>
#include <Game/Objects/Player/Structures/PlayerStructures.h>

// c++
#include <memory>
#include <optional>
#include <unordered_map>
// imgui
#include <imgui.h>

//============================================================================
//	PlayerStateController class
//============================================================================
class PlayerStateController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerStateController() = default;
	~PlayerStateController() = default;

	void Init(Player& owner);

	void Update(Player& owner);

	void ImGui(const Player& owner);

	//--------- accessor -----------------------------------------------------

	void SetBossEnemy(const BossEnemy* bossEnemy);
	void SetFollowCamera(FollowCamera* followCamera);
	void SetPostProcessSystem(PostProcessSystem* postProcessSystem);

	void SetStatas(const PlayerStats& stats) { stats_ = stats; }
	void SetForcedState(Player& owner, PlayerState state);

	PlayerState GetCurrentState() const { return current_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 入力
	std::unique_ptr<PlayerInputMapper> inputMapper_;
	// ステータス
	PlayerStats stats_;

	std::unordered_map<PlayerState, std::unique_ptr<PlayerIState>> states_;

	// 各状態の遷移条件
	std::unordered_map<PlayerState, PlayerStateCondition> conditions_;

	// 各状態の遷移クールタイム
	std::unordered_map<PlayerState, float> lastEnterTime_;
	float currentEnterTime_;

	// 受付済みコンボ
	std::optional<PlayerState> queued_;

	PlayerState current_;                  // 現在の状態
	std::optional<PlayerState> requested_; // 次の状態

	// editor
	int editingStateIndex_;
	int comboIndex_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateInputState();

	// helper
	void SetInputMapper();
	bool Request(PlayerState state);
	void ChangeState(Player& owner);
	bool CanTransition(PlayerState next, bool viaQueue) const;
	bool IsCombatState(PlayerState state) const;
	bool IsInChain() const;
};