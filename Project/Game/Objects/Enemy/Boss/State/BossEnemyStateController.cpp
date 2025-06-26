#include "BossEnemyStateController.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

// state
#include <Game/Objects/Enemy/Boss/State/BossEnemyIdleState.h>

// imgui
#include <imgui.h>

//============================================================================
//	BossEnemyStateController classMethods
//============================================================================

namespace {

	// 各状態の名前
	const char* kStatetateNames[] = {
		"Idle"
	};

	// jsonを保存するパス
	const std::string kJsonPath = "Enemy/Boss/stateParameter.json";
}

void BossEnemyStateController::Init(BossEnemy& owner) {

	// 各状態を初期化
	states_.emplace(BossEnemyState::Idle, std::make_unique<BossEnemyIdleState>());

	// json適応
	ApplyJson();

	// 初期状態を設定
	requested_ = BossEnemyState::Idle;
	ChangeState(owner);
}

void BossEnemyStateController::SetPlayer(const Player* player) {

	// 各状態にplayerをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetPlayer(player);
	}
}

void BossEnemyStateController::Update(BossEnemy& owner) {

	// 現在のフェーズの更新処理
	UpdatePhase();

	// 何か設定されて入れば状態遷移させる
	if (requested_.has_value()) {

		ChangeState(owner);
	}

	// 現在の状態を更新
	if (BossEnemyIState* currentState = states_[current_].get()) {

		currentState->Update(owner);
	}
}

void BossEnemyStateController::UpdatePhase() {

	// 現在のHP割合
	uint32_t hpRate = (stats_.currentHP * 100) / stats_.maxHP;

	// HP割合に応じて現在のフェーズを計算して設定
	currentPhase_ = 0;
	for (uint32_t threshold : stats_.hpThresholds) {
		if (hpRate < threshold) {

			// 閾値以下ならフェーズを進める
			++currentPhase_;
		}
	}
}

void BossEnemyStateController::ChangeState(BossEnemy& owner) {

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

void BossEnemyStateController::ImGui() {

	if (ImGui::Button("SaveJson...stateParameter.json")) {

		SaveJson();
	}

	ImGui::Text("currentPhase: %d / %d", currentPhase_, stats_.hpThresholds.size());

	int current = static_cast<int>(current_);
	if (ImGui::Combo("CurrentState", &current, kStatetateNames, IM_ARRAYSIZE(kStatetateNames))) {

		requested_ = static_cast<BossEnemyState>(current);
	}
	ImGui::Separator();
	if (auto* currentState = states_[current_].get()) {

		currentState->ImGui();
	}
}

void BossEnemyStateController::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(kJsonPath, data)) {
		return;
	}

	for (auto& [state, ptr] : states_) {

		ptr->ApplyJson(data[kStatetateNames[static_cast<int>(state)]]);
	}
}

void BossEnemyStateController::SaveJson() {

	Json data;
	for (auto& [state, ptr] : states_) {

		ptr->SaveJson(data[kStatetateNames[static_cast<int>(state)]]);
	}

	JsonAdapter::Save(kJsonPath, data);
}
