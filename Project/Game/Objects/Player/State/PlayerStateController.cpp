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
	requested_ = PlayerState::Idle;
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

	// 歩き、待機状態の状態遷移
	{
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

	// 遷移できるかどうか判定
	const bool result = CanTransition(state, false);
	if (!result) {
		return false;
	}

	if (auto* currentState = states_[current_].get()) {
		if (!currentState->GetCanExit()) {

			queued_ = state;
			return true;
		}
	}

	// 次の状態をリクエスト
	requested_ = state;
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
	if (itTime != lastEnterTime_.end() &&
		totalTime - itTime->second < condition.coolTime) {

		return false;
	}
	// SP比較
	if (stats_.currentSkilPoint < condition.requireSkillPoint) {
		return false;
	}

	// 強制キャンセル判定
	if (!viaQueue) {
		if (!condition.interruptableBy.empty()) {
			const bool cancel = std::ranges::find(condition.interruptableBy, current_) !=
				condition.interruptableBy.end();
			if (!cancel) {

				return false;
			}
		}
	}

	// 前状態
	if (!condition.allowedPreState.empty()) {
		const bool ok = std::ranges::find(condition.allowedPreState, current_) !=
			condition.allowedPreState.end();
		if (!ok) {

			return false;
		}
	}

	// チェイン入力判定
	if (condition.chainInputTime > 0.0f) {
		if (totalTime - currentEnterTime_ > condition.chainInputTime) {

			return false;
		}
	}
	return true;
}

void PlayerStateController::ImGui() {

	if (ImGui::Button("SaveJson...stateParameter.json")) {

		SaveJson();
	}

	// 現在の状態
	ImGui::Text("currentStat: %s", kStateNames[static_cast<uint32_t>(current_)]);

	// 各stateの値を調整
	ImGui::Combo("EditState", &editingStateIndex_, kStateNames, IM_ARRAYSIZE(kStateNames));
	ImGui::SeparatorText(kStateNames[editingStateIndex_]);

	if (const auto& state = states_[static_cast<PlayerState>(editingStateIndex_)].get()) {

		state->ImGui();
	}

	if (ImGui::CollapsingHeader("Transition Conditions")) {

		ImGui::Combo("State##cond", &comboIndex_, kStateNames, IM_ARRAYSIZE(kStateNames));
		PlayerState state = static_cast<PlayerState>(comboIndex_);
		PlayerStateCondition& condition = conditions_[state];

		ImGui::DragFloat("coolTime", &condition.coolTime, 0.01f, 0.0f);
		ImGui::DragFloat("chainInputTime", &condition.chainInputTime, 0.01f, 0.0f);
		ImGui::DragInt("needSP", &condition.requireSkillPoint, 1, 0);

		if (ImGui::TreeNode("AllowedPrev")) {
			for (int i = 0; i < IM_ARRAYSIZE(kStateNames); ++i) {

				bool has = std::ranges::find(condition.allowedPreState, static_cast<PlayerState>(i)) !=
					condition.allowedPreState.end();
				if (ImGui::CheckboxFlags(kStateNames[i], &reinterpret_cast<int&>(has), 1)) {
					if (has) {

						condition.allowedPreState.push_back(static_cast<PlayerState>(i));
					} else {

						std::erase(condition.allowedPreState, static_cast<PlayerState>(i));
					}
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("InterruptableBy")) {
			for (int i = 0; i < IM_ARRAYSIZE(kStateNames); ++i) {

				bool has = std::ranges::find(condition.interruptableBy, static_cast<PlayerState>(i)) !=
					condition.interruptableBy.end();
				if (ImGui::CheckboxFlags(kStateNames[i], &reinterpret_cast<int&>(has), 1)) {
					if (has) {

						condition.interruptableBy.push_back(static_cast<PlayerState>(i));
					} else {

						std::erase(condition.interruptableBy, static_cast<PlayerState>(i));
					}
				}
			}
			ImGui::TreePop();
		}
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

	if (!data.contains("Conditions")) return;
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