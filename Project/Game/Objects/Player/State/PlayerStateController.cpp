#include "PlayerStateController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/JsonAdapter.h>

// inputDevice
#include <Game/Objects/Player/Input/Device/PlayerKeyInput.h>
#include <Game/Objects/Player/Input/Device/PlayerGamePadInput.h>

// state
#include <Game/Objects/Player/State/States/PlayerIdleState.h>
#include <Game/Objects/Player/State/States/PlayerWalkState.h>
#include <Game/Objects/Player/State/States/PlayerDashState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_1stState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_2ndState.h>
#include <Game/Objects/Player/State/States/PlayerAttack_3rdState.h>
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
		"SkilAttack","SpecialAttack","Parry",
	};

	// jsonを保存するパス
	const std::string kStateJsonPath = "Player/stateParameter.json";
}

void PlayerStateController::Init(Player& owner) {

	// 入力クラスを初期化
	Input* input = Input::GetInstance();
	inputMapper_ = std::make_unique<PlayerInputMapper>();
	inputMapper_->AddDevice(std::make_unique<PlayerKeyInput>(input));
	inputMapper_->AddDevice(std::make_unique<PlayerGamePadInput>(input));

	// 各状態を初期化
	states_.emplace(PlayerState::Idle, std::make_unique<PlayerIdleState>());
	states_.emplace(PlayerState::Walk, std::make_unique<PlayerWalkState>());
	states_.emplace(PlayerState::Dash, std::make_unique<PlayerDashState>());
	states_.emplace(PlayerState::Attack_1st, std::make_unique<PlayerAttack_1stState>());
	states_.emplace(PlayerState::Attack_2nd, std::make_unique<PlayerAttack_2ndState>());
	states_.emplace(PlayerState::Attack_3rd, std::make_unique<PlayerAttack_3rdState>());
	states_.emplace(PlayerState::SkilAttack, std::make_unique<PlayerSkilAttackState>());
	states_.emplace(PlayerState::SpecialAttack, std::make_unique<PlayerSpecialAttackState>());
	states_.emplace(PlayerState::Parry, std::make_unique<PlayerParryState>());
	// inputを設定
	SetInputMapper();

	// json適応
	ApplyJson();

	// 初期状態を設定
	current_ = PlayerState::Idle;
	requested_ = PlayerState::Idle;
	currentEnterTime_ = GameTimer::GetTotalTime();
	lastEnterTime_[current_] = currentEnterTime_;
	ChangeState(owner);
}

void PlayerStateController::SetInputMapper() {

	// 各状態にinputをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetInputMapper(inputMapper_.get());
	}
}

void PlayerStateController::SetBossEnemy(const BossEnemy* bossEnemy) {

	// 各状態にbossEnemyをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetBossEnemy(bossEnemy);
	}
}

void PlayerStateController::SetFollowCamera(const FollowCamera* followCamera) {

	// 各状態にfollowCameraをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetFollowCamera(followCamera);
	}
}

void PlayerStateController::Update(Player& owner) {

	// 入力に応じた状態の遷移
	UpdateInputState();

	// 何か設定されて入れば状態遷移させる
	if (queued_) {
		if (states_[current_]->GetCanExit() && CanTransition(*queued_, true)) {

			requested_ = queued_;
			queued_.reset();
		}
		// 条件外の場合はリセットする
		else if (states_[current_]->GetCanExit()) {

			queued_.reset();
		}
	}

	// 何か設定されていれば遷移させる
	if (requested_.has_value()) {

		ChangeState(owner);
	}

	// 現在の状態を更新
	if (PlayerIState* currentState = states_[current_].get()) {

		currentState->Update(owner);
	}
}

void PlayerStateController::UpdateInputState() {

	// コンボ中は判定をスキップする
	const bool inCombat = IsCombatState(current_);
	const bool canExit = states_.at(current_)->GetCanExit();
	const bool isInChain = IsInChain();
	const bool actionLocked = (inCombat && !canExit) || (inCombat && isInChain);

	// 歩き、待機状態の状態遷移
	{
		if (!actionLocked) {
			// 移動方向
			const Vector2 move(inputMapper_->GetVector(PlayerAction::MoveX),
				inputMapper_->GetVector(PlayerAction::MoveZ));

			// 動いたかどうか判定
			const bool isMove = move.Length() > std::numeric_limits<float>::epsilon();
			// 移動していた場合は歩き、していなければ待機状態のまま
			if (isMove) {

				Request(PlayerState::Walk);
			} else {

				Request(PlayerState::Idle);
			}
		}
	}

	// ダッシュ、攻撃の状態遷移
	{
		if (inputMapper_->IsPressed(PlayerAction::Dash)) {

			Request(PlayerState::Dash);
		} else {
			// ダッシュ中に離したら待機状態にする
			if (current_ == PlayerState::Dash) {

				Request(PlayerState::Idle);
			}
		}

		if (inputMapper_->IsTriggered(PlayerAction::Attack)) {

			// dash -> 1段
			if (current_ == PlayerState::Dash) {

				Request(PlayerState::Attack_1st);
			}
			// 1段 -> 2段
			else if (current_ == PlayerState::Attack_1st) {
				Request(PlayerState::Attack_2nd);
			}
			// 2段 -> 3段
			else if (current_ == PlayerState::Attack_2nd) {
				Request(PlayerState::Attack_3rd);
			}
			// 1段目
			else {
				Request(PlayerState::Attack_1st);
			}
		}

		// スキル攻撃
		if (inputMapper_->IsTriggered(PlayerAction::Skill)) {

			Request(PlayerState::SkilAttack);
		}
		// 必殺
		if (inputMapper_->IsTriggered(PlayerAction::Special)) {

			Request(PlayerState::SpecialAttack);
		}
	}
}

bool PlayerStateController::Request(PlayerState state) {

	// 現在の状態と同じなら何もしない
	if (state == current_ && !states_.at(current_)->GetCanExit()) {
		return false;
	}
	if (queued_ && *queued_ == state) {
		return true;
	}

	// 遷移できるかどうか判定
	const bool result = CanTransition(state, false);
	if (!result) {
		return false;
	}

	// 遷移可能か、現在の状態が終了可能かチェック
	if (!states_.at(current_)->GetCanExit()) {

		queued_ = state;
	} else {

		requested_ = state;
	}
	return true;
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

	currentEnterTime_ = GameTimer::GetTotalTime();
	lastEnterTime_[current_] = currentEnterTime_;
}

bool PlayerStateController::CanTransition(PlayerState next, bool viaQueue) const {

	const auto it = conditions_.find(next);
	if (it == conditions_.end()) {
		return true;
	}

	const PlayerStateCondition& condition = it->second;
	const float totalTime = GameTimer::GetTotalTime();

	// クールタイムの処理
	auto itTime = lastEnterTime_.find(next);
	// クールタイムが終わっていなければ遷移不可
	if (itTime != lastEnterTime_.end() && totalTime - itTime->second < condition.coolTime) {

		return false;
	}
	// SP比較
	if (stats_.currentSkilPoint < condition.requireSkillPoint) {
		return false;
	}

	// 強制キャンセルを行えるか判定
	if (!viaQueue) {
		if (!condition.interruptableBy.empty()) {
			const bool cancel = std::ranges::find(
				condition.interruptableBy, current_) != condition.interruptableBy.end();
			if (!cancel) {

				return false;
			}
		}
	}

	// 遷移可能な前状態かチェック
	if (!condition.allowedPreState.empty()) {
		const bool ok = std::ranges::find(condition.allowedPreState, current_) !=
			condition.allowedPreState.end();
		if (!ok) {

			return false;
		}
	}

	// コンボ入力判定
	if (!viaQueue && condition.chainInputTime > 0.0f) {
		if (totalTime - currentEnterTime_ > condition.chainInputTime) {
			return false;
		}
	}
	return true;
}

bool PlayerStateController::IsCombatState(PlayerState state) const {

	switch (state) {
	case PlayerState::Attack_1st:
	case PlayerState::Attack_2nd:
	case PlayerState::Attack_3rd:
	case PlayerState::SkilAttack:
	case PlayerState::SpecialAttack:
	case PlayerState::Parry:
		return true;
	default:
		return false;
	}
}

bool PlayerStateController::IsInChain() const {

	auto it = conditions_.find(current_);
	if (it == conditions_.end()) {
		return false;
	}

	const float elapsed = GameTimer::GetTotalTime() - currentEnterTime_;
	return (it->second.chainInputTime > 0.0f) && (elapsed <= it->second.chainInputTime);
}

void PlayerStateController::ImGui(const Player& owner) {

	// tool
	ImGui::Text("Current : %s", kStateNames[static_cast<int>(current_)]);
	ImGui::SameLine();
	if (ImGui::Button("Save##StateJson")) {
		SaveJson();
	}

	// main
	if (ImGui::BeginTabBar("PStateTabs")) {

		// ---- Runtime -------------------------------------------------
		if (ImGui::BeginTabItem("Runtime")) {
			ImGui::Text("Enter Time   : %.2f", currentEnterTime_);
			ImGui::Text("Queued State : %s",
				queued_ ? kStateNames[static_cast<int>(*queued_)] : "None");
			ImGui::EndTabItem();
		}

		// ---- States --------------------------------------------------
		if (ImGui::BeginTabItem("States")) {
			ImGui::BeginChild("StateList", ImVec2(140, 0), true);
			for (int i = 0; i < IM_ARRAYSIZE(kStateNames); ++i) {
				bool selected = (editingStateIndex_ == i);
				if (ImGui::Selectable(kStateNames[i], selected)) {
					editingStateIndex_ = i;
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("StateDetail", ImVec2(0, 0), true);
			if (auto* st = states_[static_cast<PlayerState>(editingStateIndex_)].get()) {
				st->ImGui(owner);
			}
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		// ---- Conditions ---------------------------------------------
		if (ImGui::BeginTabItem("Conditions")) {
			ImGui::Combo("Edit##cond-state",
				&comboIndex_,
				kStateNames,
				IM_ARRAYSIZE(kStateNames));

			PlayerState            st = static_cast<PlayerState>(comboIndex_);
			PlayerStateCondition& cond = conditions_[st];

			ImGui::DragFloat("CoolTime", &cond.coolTime, 0.01f, 0.0f);
			ImGui::DragFloat("InputWindow", &cond.chainInputTime, 0.01f, 0.0f);
			ImGui::DragInt("Need SP", &cond.requireSkillPoint, 1, 0);

			// Allowed / Interruptable をテーブルで
			if (ImGui::BeginTable("CondTable", 3, ImGuiTableFlags_Borders)) {
				ImGui::TableSetupColumn("State");
				ImGui::TableSetupColumn("Allowed");
				ImGui::TableSetupColumn("Interrupt");
				ImGui::TableHeadersRow();

				for (int i = 0; i < IM_ARRAYSIZE(kStateNames); ++i) {
					PlayerState s = static_cast<PlayerState>(i);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(kStateNames[i]);

					// ---- Allowed 列 -------------------------------------------------
					ImGui::TableNextColumn();
					{
						bool allowed = std::ranges::find(cond.allowedPreState, s)
							!= cond.allowedPreState.end();
						std::string id = "##allow_" + std::to_string(i);
						if (ImGui::Checkbox(id.c_str(), &allowed)) {
							if (allowed) {
								cond.allowedPreState.push_back(s);
							} else {
								std::erase(cond.allowedPreState, s);
							}
						}
					}

					// ---- Interrupt 列 -----------------------------------------------
					ImGui::TableNextColumn();
					{
						bool intr = std::ranges::find(cond.interruptableBy, s)
							!= cond.interruptableBy.end();
						std::string id = "##intr_" + std::to_string(i);
						if (ImGui::Checkbox(id.c_str(), &intr)) {
							if (intr) {
								cond.interruptableBy.push_back(s);
							} else {
								std::erase(cond.interruptableBy, s);
							}
						}
					}
				}
				ImGui::EndTable();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
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

	if (!data.contains("Conditions")) {
		return;
	}
	const Json& condRoot = data["Conditions"];
	for (auto& [state, ptr] : states_) {

		const char* key = kStateNames[static_cast<int>(state)];
		if (!condRoot.contains(key)) {
			continue;
		}

		PlayerStateCondition condition;
		condition.FromJson(condRoot[key]);
		conditions_[state] = std::move(condition);
	}
}

void PlayerStateController::SaveJson() {

	Json data;
	for (auto& [state, ptr] : states_) {

		ptr->SaveJson(data[kStateNames[static_cast<int>(state)]]);
	}

	Json& condRoot = data["Conditions"];
	for (auto& [state, cond] : conditions_) {

		cond.ToJson(condRoot[kStateNames[static_cast<int>(state)]]);
	}

	JsonAdapter::Save(kStateJsonPath, data);
}