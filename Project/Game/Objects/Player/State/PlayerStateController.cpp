#include "PlayerStateController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/JsonAdapter.h>

// state
#include <Game/Objects/Player/State/States/PlayerIdleState.h>
#include <Game/Objects/Player/State/States/PlayerWalkState.h>
#include <Game/Objects/Player/State/States/PlayerDashState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_1stState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_2ndState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_3rdState.h>
#include <Game/Objects/Player/State/States/PlayerDashAttackState.h>
#include <Game/Objects/Player/State/States/PlayerSkilAttackState.h>
#include <Game/Objects/Player/State/States/PlayerSpecialAttackState.h>
#include <Game/Objects/Player/State/States/PlayerParryState.h>

//============================================================================
//	PlayerStateController classMethods
//============================================================================

namespace {

	// 各状態の名前
	const char* kStateNames[] = {
		"Idle","Walk","Dash","Attack_1st","Attack_2nd","Attack_3rd",
		"DashAttack","SkilAttack","SpecialAttack","Parry",
	};

	// jsonを保存するパス
	const std::string kStateJsonPath = "Player/stateParameter.json";
}

void PlayerStateController::Init(Player& owner) {

	// 各状態を初期化
	states_.emplace(PlayerState::Idle, std::make_unique<PlayerIdleState>());
	states_.emplace(PlayerState::Walk, std::make_unique<PlayerWalkState>());
	states_.emplace(PlayerState::Dash, std::make_unique<PlayerDashState>());
	states_.emplace(PlayerState::Attack_1st, std::make_unique<PlayerAttack_1stState>());
	states_.emplace(PlayerState::Attack_2nd, std::make_unique<PlayerAttack_2ndState>());
	states_.emplace(PlayerState::Attack_3rd, std::make_unique<PlayerAttack_3rdState>());
	states_.emplace(PlayerState::DashAttack, std::make_unique<PlayerDashAttackState>());
	states_.emplace(PlayerState::SkilAttack, std::make_unique<PlayerSkilAttackState>());
	states_.emplace(PlayerState::SpecialAttack, std::make_unique<PlayerSpecialAttackState>());
	states_.emplace(PlayerState::Parry, std::make_unique<PlayerParryState>());

	// json適応
	ApplyJson();

	// 初期状態を設定
	requested_ = PlayerState::Idle;
	ChangeState(owner);
}

void PlayerStateController::SetBossEnemy(const BossEnemy* bossEnemy) {

	// 各状態にbossEnemyをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetBossEnemy(bossEnemy);
	}
}

void PlayerStateController::Update(Player& owner) {

	// 何か設定されて入れば状態遷移させる
	if (requested_.has_value()) {

		ChangeState(owner);
	}

	// 現在の状態を更新
	if (PlayerIState* currentState = states_[current_].get()) {

		currentState->Update(owner);
	}
}

void PlayerStateController::ChangeState(Player& owner) {

	// 同じなら遷移させない
	if (requested_.value() == current_) {
		requested_ = std::nullopt;
		return;
	}

	// 現在の状態の終了処理
	if (auto* currentState = states_[current_].get()) {

		currentState->Exit(owner);
	}

	// 次の状態を設定する
	current_ = requested_.value();

	// 次の状態を初期化する
	if (auto* currentState = states_[current_].get()) {

		currentState->Enter(owner);
	}
}

void PlayerStateController::ImGui() {

	if (ImGui::Button("SaveJson...stateParameter.json")) {

		SaveJson();
	}

	// 各stateの値を調整
	editingStateIndex_ = 0;
	ImGui::Combo("EditState", &editingStateIndex_, kStateNames, IM_ARRAYSIZE(kStateNames));
	ImGui::SeparatorText(kStateNames[editingStateIndex_]);

	if (const auto& state = states_[static_cast<PlayerState>(editingStateIndex_)].get()) {

		state->ImGui();
	}
}

void PlayerStateController::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(kStateJsonPath, data)) {
		return;
	}

	for (auto& [state, ptr] : states_) {

		ptr->ApplyJson(data[kStateNames[static_cast<int>(state)]]);
	}
}

void PlayerStateController::SaveJson() {

	Json data;
	for (auto& [state, ptr] : states_) {

		ptr->SaveJson(data[kStateNames[static_cast<int>(state)]]);
	}

	JsonAdapter::Save(kStateJsonPath, data);
}