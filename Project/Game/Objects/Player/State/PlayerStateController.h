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

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetBossEnemy(const BossEnemy* bossEnemy);

	void SetStatas(const PlayerStats& stats) { stats_ = stats; }
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

	PlayerState current_;                  // 現在の状態
	std::optional<PlayerState> requested_; // 次の状態

	// editor
	int editingStateIndex_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();
	
	// update
	void UpdateInputState();

	// helper
	bool Request(PlayerState state);
	void ChangeState(Player& owner);
};