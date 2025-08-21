#include "PlayerStateController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Engine/Utility/GameTimer.h>
#include <Engine/Utility/JsonAdapter.h>
#include <Engine/Utility/EnumAdapter.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

// inputDevice
#include <Game/Objects/GameScene/Player/Input/Device/PlayerKeyInput.h>
#include <Game/Objects/GameScene/Player/Input/Device/PlayerGamePadInput.h>

// state
#include <Game/Objects/GameScene/Player/State/States/PlayerIdleState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerWalkState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerDashState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerAvoidSatate.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerAttack_1stState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerAttack_2ndState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerAttack_3rdState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerSkilAttackState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerParryState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerSwitchAllyState.h>
#include <Game/Objects/GameScene/Player/State/States/PlayerStunAttackState.h>

//============================================================================
//	PlayerStateController classMethods
//============================================================================

void PlayerStateController::Init(Player& owner) {

	// 入力クラスを初期化
	Input* input = Input::GetInstance();
	inputMapper_ = std::make_unique<InputMapper<PlayerInputAction>>();
	//inputMapper_->AddDevice(std::make_unique<PlayerKeyInput>(input));
	inputMapper_->AddDevice(std::make_unique<PlayerGamePadInput>(input));

	// 各状態を初期化
	states_.emplace(PlayerState::Idle, std::make_unique<PlayerIdleState>());
	states_.emplace(PlayerState::Walk, std::make_unique<PlayerWalkState>());
	states_.emplace(PlayerState::Dash, std::make_unique<PlayerDashState>());
	states_.emplace(PlayerState::Avoid, std::make_unique<PlayerAvoidSatate>());
	states_.emplace(PlayerState::Attack_1st, std::make_unique<PlayerAttack_1stState>());
	states_.emplace(PlayerState::Attack_2nd, std::make_unique<PlayerAttack_2ndState>());
	states_.emplace(PlayerState::Attack_3rd, std::make_unique<PlayerAttack_3rdState>());
	states_.emplace(PlayerState::SkilAttack, std::make_unique<PlayerSkilAttackState>());
	states_.emplace(PlayerState::Parry, std::make_unique<PlayerParryState>());
	states_.emplace(PlayerState::SwitchAlly, std::make_unique<PlayerSwitchAllyState>());
	states_.emplace(PlayerState::StunAttack, std::make_unique<PlayerStunAttackState>(owner.GetAlly()));

	// json適応
	ApplyJson();

	// inputを設定
	SetInputMapper();
	// 状態間の値の共有
	SetStateValue();

	// 初期状態を設定
	current_ = PlayerState::Idle;
	requested_ = PlayerState::Idle;
	currentEnterTime_ = GameTimer::GetTotalTime();
	lastEnterTime_[current_] = currentEnterTime_;
	ChangeState(owner);
}

void PlayerStateController::SetStateValue() {

	// 状態間の値の共有(値ずれを防ぐため)
	static_cast<PlayerIdleState*>(states_.at(PlayerState::Idle).get())->SetBlurParam(
		static_cast<PlayerSwitchAllyState*>(states_.at(PlayerState::SwitchAlly).get())->GetBlurParam());

	static_cast<PlayerStunAttackState*>(states_.at(PlayerState::StunAttack).get())->SetBlurParam(
		static_cast<PlayerSwitchAllyState*>(states_.at(PlayerState::SwitchAlly).get())->GetBlurParam());
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
	bossEnemy_ = nullptr;
	bossEnemy_ = bossEnemy;
}

void PlayerStateController::SetFollowCamera(FollowCamera* followCamera) {

	// 各状態にfollowCameraをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetFollowCamera(followCamera);
	}
}

void PlayerStateController::SetForcedState(Player& owner, PlayerState state) {

	// 同じ状態へは遷移不可
	if (current_ == state) {
		// パリィのみ例外
		if (state == PlayerState::Parry) {
			if (const auto& currentState = states_[current_].get()) {

				currentState->Exit(owner);
				currentState->Enter(owner);
			}
			currentEnterTime_ = GameTimer::GetTotalTime();
			lastEnterTime_[current_] = currentEnterTime_;
			owner.GetAttackCollision()->SetEnterState(current_);
		}
		return;
	}

	// 全ての行動を削除
	queued_.reset();
	requested_.reset();

	// 現在の行動を強制終了
	if (auto* currentState = states_[current_].get()) {

		currentState->Exit(owner);
	}

	// 次の状態を設定
	PlayerState preState = current_;
	current_ = state;

	// 遷移入り
	if (auto* currentState = states_[current_].get()) {

		currentState->SetPreState(preState);
		currentState->Enter(owner);
	}

	// 現在の時間を記録
	currentEnterTime_ = GameTimer::GetTotalTime();
	lastEnterTime_[current_] = currentEnterTime_;
	owner.GetAttackCollision()->SetEnterState(current_);
}

PlayerState PlayerStateController::GetSwitchSelectState() const {

	// SwitchAlly状態の時になにをplayerが選択したのか取得する
	return static_cast<PlayerSwitchAllyState*>(states_.at(PlayerState::SwitchAlly).get())->GetSelectState();
}

void PlayerStateController::Update(Player& owner) {

	// 入力に応じた状態の遷移
	UpdateInputState();

	// パリィ処理
	UpdateParryState(owner);

	// 何か予約設定されて入れば状態遷移させる
	if (queued_) {
		bool canInterrupt = false;
		if (auto it = conditions_.find(*queued_); it != conditions_.end()) {

			canInterrupt = it->second.CheckInterruptableByState(current_);
		}

		if ((states_[current_]->GetCanExit() || canInterrupt) &&
			CanTransition(*queued_, !canInterrupt)) {

			requested_ = queued_;
			queued_.reset();
		}
	}

	// 何か設定されていれば遷移させる
	if (requested_.has_value()) {

		ChangeState(owner);
	}

	// パリィの状態管理
	RequestParryState();

	// 敵がスタン中の状態遷移処理
	HandleStunTransition(owner);

	// 現在の状態を更新
	if (PlayerIState* currentState = states_[current_].get()) {

		currentState->Update(owner);
	}
}

void PlayerStateController::UpdateInputState() {

	// スタン処理中は状態遷移不可
	if (IsStunProcessing()) {
		return;
	}

	// コンボ中は判定をスキップする
	const bool inCombat = IsCombatState(current_);
	const bool canExit = states_.at(current_)->GetCanExit();
	const bool isInChain = IsInChain();
	const bool actionLocked = (inCombat && !canExit) || (inCombat && isInChain);

	// 歩き、待機状態の状態遷移
	{
		if (!actionLocked) {
			// 移動方向
			const Vector2 move(inputMapper_->GetVector(PlayerInputAction::MoveX),
				inputMapper_->GetVector(PlayerInputAction::MoveZ));

			// 動いたかどうか判定
			const bool isMove = move.Length() > std::numeric_limits<float>::epsilon();
			// 移動していた場合は歩き、していなければ待機状態のまま
			if (isMove) {

				Request(PlayerState::Walk);
			} else {

				Request(PlayerState::Idle);

				// 移動していないときに回避を押したら回避に遷移させる
				if (inputMapper_->IsTriggered(PlayerInputAction::Avoid)) {

					Request(PlayerState::Avoid);
					return;
				}
			}
		}
	}

	// ダッシュ、攻撃の状態遷移
	{
		if (inputMapper_->IsPressed(PlayerInputAction::Dash)) {

			Request(PlayerState::Dash);
		} else {
			// ダッシュ中に離したら待機状態にする
			if (current_ == PlayerState::Dash) {

				Request(PlayerState::Idle);
			}
		}

		if (inputMapper_->IsTriggered(PlayerInputAction::Attack)) {

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
		if (inputMapper_->IsTriggered(PlayerInputAction::Skill)) {

			Request(PlayerState::SkilAttack);
		}
	}

	// パリィの入力判定
	if (inputMapper_->IsTriggered(PlayerInputAction::Parry)) {

		const ParryParameter& parryParam = bossEnemy_->GetParryParam();
		if (parryParam.canParry) {

			// 入力があればパリィ処理を予約する
			parrySession_ = {};
			parrySession_.done = 0;
			parrySession_.active = true;
			parrySession_.reserved = true;
			parrySession_.total = std::max<uint32_t>(1, parryParam.continuousCount);
			parrySession_.reservedStart = GameTimer::GetTotalTime();
		}
	}
}

void PlayerStateController::UpdateParryState(Player& owner) {

	// 敵がパリィ可能かどうかチェック
	if (parrySession_.active && parrySession_.reserved &&
		bossEnemy_ && const_cast<BossEnemy*>(bossEnemy_)->ConsumeParryTiming()) {

		++parrySession_.done;
		const bool isLast = (parrySession_.done >= parrySession_.total);
		if (const auto& parryState = static_cast<PlayerParryState*>(states_.at(PlayerState::Parry).get())) {

			parryState->SetAllowAttack(isLast);
		}

		// パリィ状態に強制遷移させる
		SetForcedState(owner, PlayerState::Parry);
		parrySession_.reserved = !isLast;
		parrySession_.reservedStart = GameTimer::GetTotalTime();

		return;
	}

	// 予約を時間経過で削除
	if (parrySession_.active && parrySession_.reserved) {

		const bool windowClosed = bossEnemy_ && !bossEnemy_->GetParryParam().canParry;
		const float now = GameTimer::GetTotalTime();
		const float reservedEnd = 1.0f;
		// 予約失効チェック
		bool timeout = false;
		if (parrySession_.total == 1) {

			timeout = (now - parrySession_.reservedStart) > reservedEnd;
		} else {

			timeout = (now - parrySession_.reservedStart) > reservedEnd * static_cast<float>(parrySession_.total);
		}

		if (windowClosed || timeout) {

			parrySession_.Init();
		}
	}
}

void PlayerStateController::RequestParryState() {

	if (current_ == PlayerState::Parry && states_[current_]->GetCanExit()) {
		if (parrySession_.done < parrySession_.total) {

			parrySession_.reservedStart = GameTimer::GetTotalTime();
		} else {

			parrySession_.Init();
		}
	}
}

bool PlayerStateController::Request(PlayerState state) {

	// 現在の状態と同じなら何もしない
	if (state == current_) {
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

	// 強制遷移可能先かチェックする
	bool canInterrupt = false;
	if (auto it = conditions_.find(state); it != conditions_.end()) {

		canInterrupt = it->second.CheckInterruptableByState(current_);
	}

	// 遷移可能か、現在の状態が終了可能かチェック
	if (!states_.at(current_)->GetCanExit() && !canInterrupt) {

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
	PlayerState preState = current_;
	current_ = requested_.value();

	// 次の状態を初期化する
	if (auto* currentState = states_[current_].get()) {

		currentState->SetPreState(preState);
		currentState->Enter(owner);
	}

	currentEnterTime_ = GameTimer::GetTotalTime();
	lastEnterTime_[current_] = currentEnterTime_;

	// player側に切り替えを通知する
	owner.GetAttackCollision()->SetEnterState(current_);
}

void PlayerStateController::HandleStunTransition(Player& owner) {

	// 切り替え処理
	if (current_ == PlayerState::SwitchAlly &&
		states_[current_]->GetCanExit()) {

		// 選択した状態へ遷移させる
		SetForcedState(owner, GetSwitchSelectState());
		return;
	}

	// スタン攻撃の終了判定
	if (current_ == PlayerState::StunAttack &&
		states_[current_]->GetCanExit()) {

		// 攻撃が終わったらアイドル状態に戻す
		SetForcedState(owner, PlayerState::Idle);
		return;
	}
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

bool PlayerStateController::IsStunProcessing() const {

	return current_ == PlayerState::SwitchAlly ||
		current_ == PlayerState::StunAttack;
}

void PlayerStateController::ParrySession::Init() {

	active = false;    // 処理中か
	reserved = false;  // タイミング待ち
	total = 0; // 連続回数
	done = 0;  // 処理済み回数
	reservedStart = 0.0f;
}

void PlayerStateController::ImGui(const Player& owner) {

	// tool
	ImGui::Text("Current : %s", EnumAdapter<PlayerState>::ToString(current_));
	ImGui::SameLine();
	if (ImGui::Button("Save##StateJson")) {
		SaveJson();
	}

	// main
	if (ImGui::BeginTabBar("PStateTabs")) {

		// ---- Runtime -------------------------------------------------
		if (ImGui::BeginTabItem("Runtime")) {

			ImGui::Text("Enter Time   : %.2f", currentEnterTime_);
			ImGui::Text("Queued State : %s", queued_ ? EnumAdapter<PlayerState>::ToString(*queued_) : "None");

			ImGui::SeparatorText("Parry");

			ImGui::Text(std::format("active: {}", parrySession_.active).c_str());
			ImGui::Text(std::format("reserved: {}", parrySession_.reserved).c_str());
			ImGui::Text(std::format("total: {}", parrySession_.total).c_str());

			ImGui::EndTabItem();
		}

		// ---- States --------------------------------------------------
		if (ImGui::BeginTabItem("States")) {
			ImGui::BeginChild("StateList", ImVec2(140, 0), true);
			for (uint32_t i = 0; i < EnumAdapter<PlayerState>::GetEnumCount(); ++i) {

				bool selected = (editingStateIndex_ == static_cast<int>(i));
				if (ImGui::Selectable(EnumAdapter<PlayerState>::GetEnumName(i), selected)) {

					editingStateIndex_ = static_cast<int>(i);
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
				EnumAdapter<PlayerState>::GetEnumArray().data(),
				static_cast<int>(EnumAdapter<PlayerState>::GetEnumCount()));

			PlayerState state = static_cast<PlayerState>(comboIndex_);
			PlayerStateCondition& cond = conditions_[state];

			ImGui::DragFloat("CoolTime", &cond.coolTime, 0.01f, 0.0f);
			ImGui::DragFloat("InputWindow", &cond.chainInputTime, 0.01f, 0.0f);

			// Allowed / Interruptable をテーブルで
			if (ImGui::BeginTable("CondTable", 3, ImGuiTableFlags_Borders)) {
				ImGui::TableSetupColumn("State");
				ImGui::TableSetupColumn("Allowed");
				ImGui::TableSetupColumn("Interrupt");
				ImGui::TableHeadersRow();

				for (int i = 0; i < EnumAdapter<PlayerState>::GetEnumCount(); ++i) {
					PlayerState s = static_cast<PlayerState>(i);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted(EnumAdapter<PlayerState>::GetEnumName(i));

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

	// state
	{
		Json data;
		if (!JsonAdapter::LoadCheck(kStateJsonPath_, data)) {
			return;
		}

		for (auto& [state, ptr] : states_) {
			if (state == PlayerState::None) {
				continue;
			}

			ptr->ApplyJson(data[EnumAdapter<PlayerState>::ToString(state)]);
		}

		if (!data.contains("Conditions")) {
			return;
		}
		const Json& condRoot = data["Conditions"];
		for (auto& [state, ptr] : states_) {
			if (state == PlayerState::None) {
				continue;
			}

			const char* key = EnumAdapter<PlayerState>::ToString(state);
			if (!condRoot.contains(key)) {
				continue;
			}

			PlayerStateCondition condition;
			condition.FromJson(condRoot[key]);
			conditions_[state] = std::move(condition);
		}
	}
}

void PlayerStateController::SaveJson() {

	Json data;
	for (auto& [state, ptr] : states_) {
		if (state == PlayerState::None) {
			continue;
		}

		ptr->SaveJson(data[EnumAdapter<PlayerState>::ToString(state)]);
	}

	Json& condRoot = data["Conditions"];
	for (auto& [state, cond] : conditions_) {
		if (state == PlayerState::None) {
			continue;
		}

		cond.ToJson(condRoot[EnumAdapter<PlayerState>::ToString(state)]);
	}

	JsonAdapter::Save(kStateJsonPath_, data);
}