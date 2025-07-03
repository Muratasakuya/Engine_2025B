#include "FollowCameraStateController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Lib/Adapter/JsonAdapter.h>

// inputDevice
#include <Game/Camera/Follow/Input/Device/FollowCameraKeyInput.h>
#include <Game/Camera/Follow/Input/Device/FollowCameraGamePadInput.h>

// state
#include <Game/Camera/Follow/State/States/FollowCameraFollowState.h>
#include <Game/Camera/Follow/State/States/FollowCameraShakeState.h>
#include <Game/Camera/Follow/State/States/FollowCameraSwitchAllyState.h>
#include <Game/Camera/Follow/State/States/FollowCameraReturnState.h>
#include <Game/Camera/Follow/State/States/FollowCameraStunAttackState.h>

// imgui
#include <imgui.h>

//============================================================================
//	FollowCameraStateController classMethods
//============================================================================

namespace {

	// 各状態の名前
	const char* kStateNames[] = {
		"Follow","Shake","SwitchAlly","Return","StunAttack"
	};

	// jsonを保存するパス
	const std::string kStateJsonPath = "FollowCamera/stateParameter.json";
}

void FollowCameraStateController::Init() {

	// 入力クラスを初期化
	Input* input = Input::GetInstance();
	inputMapper_ = std::make_unique<FollowCameraInputMapper>();
	//inputMapper_->AddDevice(std::make_unique<FollowCameraKeyInput>(input));
	inputMapper_->AddDevice(std::make_unique<FollowCameraGamePadInput>(input));

	// 各状態を初期化
	states_.emplace(FollowCameraState::Follow, std::make_unique<FollowCameraFollowState>());
	states_.emplace(FollowCameraState::Shake, std::make_unique<FollowCameraShakeState>());
	states_.emplace(FollowCameraState::SwitchAlly, std::make_unique<FollowCameraSwitchAllyState>());
	states_.emplace(FollowCameraState::Return, std::make_unique<FollowCameraReturnState>());
	states_.emplace(FollowCameraState::StunAttack, std::make_unique<FollowCameraStunAttackState>());
	// inputを設定
	SetInputMapper();

	// json適応
	ApplyJson();

	// 最初の状態を設定
	current_ = FollowCameraState::Follow;
	requested_ = FollowCameraState::Follow;
	ChangeState();
}

void  FollowCameraStateController::SetTarget(const Transform3DComponent& target) {

	// 各状態にtargetをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetTarget(target);
	}
}

void FollowCameraStateController::SetInputMapper() {

	// 各状態にinputをセット
	for (const auto& state : std::views::values(states_)) {

		state->SetInputMapper(inputMapper_.get());
	}
}

void FollowCameraStateController::UpdateState() {


}

void FollowCameraStateController::Update(FollowCamera& owner) {

	// 状態遷移のリクエストを更新
	UpdateState();

	// 何か設定されていれば遷移させる
	if (requested_.has_value()) {

		ChangeState();
	}

	// 現在の状態を更新
	if (FollowCameraIState* state = states_[current_].get()) {

		state->Update(owner);
	}
}

bool FollowCameraStateController::Request(FollowCameraState state) {

	// 現在の状態と同じなら何もしない
	if (state == current_ && !states_.at(current_)->GetCanExit()) {
		return false;
	}

	// 遷移可能か、現在の状態が終了可能かチェック
	if (states_.at(current_)->GetCanExit()) {

		requested_ = state;
	}
	return true;
}

void FollowCameraStateController::ChangeState() {

	// 同じ状態なら遷移させない
	if (requested_.value() == current_) {
		requested_ = std::nullopt;
		return;
	}

	// 現在の状態の終了処理
	if (auto* currentState = states_[current_].get()) {

		currentState->Exit();
	}

	// 次の状態を設定する
	current_ = requested_.value();

	// 次の状態を初期化する
	if (auto* currentState = states_[current_].get()) {

		currentState->Enter();
	}
}

void FollowCameraStateController::ImGui(const FollowCamera& owner) {

	if (ImGui::Button("Save##StateJson")) {
		SaveJson();
	}

	ImGui::Combo("##StateCombo", &editingStateIndex_, kStateNames, IM_ARRAYSIZE(kStateNames));
	states_[static_cast<FollowCameraState>(editingStateIndex_)]->ImGui(owner);
}

void FollowCameraStateController::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(kStateJsonPath, data)) {
		return;
	}

	for (auto& [state, ptr] : states_) {

		ptr->ApplyJson(data[kStateNames[static_cast<int>(state)]]);
	}
}

void FollowCameraStateController::SaveJson() {

	Json data;
	for (auto& [state, ptr] : states_) {

		ptr->SaveJson(data[kStateNames[static_cast<int>(state)]]);
	}

	JsonAdapter::Save(kStateJsonPath, data);
}