#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Enemy/Boss/State/Interface/BossEnemyIState.h>
#include <Game/Object3D/Enemy/Boss/Structures/BossEnemyStructures.h>

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

	BossEnemyState GetCurrentState() const { return current_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<BossEnemyState, std::unique_ptr<BossEnemyIState>> states_;

	BossEnemyState current_;                  // 現在の状態
	std::optional<BossEnemyState> requested_; // 次の状態

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// helper
	void ChangeState(BossEnemy& owner);
};