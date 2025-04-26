#include "PlayerBehaviorController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerBehaviorController classMethods
//============================================================================

void PlayerBehaviorController::Init() {

	input_ = nullptr;
	input_ = Input::GetInstance();
}

void PlayerBehaviorController::Update() {

	// 歩き処理
	MoveWalk();
	// ダッシュ処理
	MoveDash();
	// 待機モーションチェック
	CheckWait();

	// behaviourを設定する
	BehaviourRequest();
}

void PlayerBehaviorController::MoveWalk() {

	// WASDどれか押していたら
	bool inputKey =
		input_->PushKey(DIK_W) ||
		input_->PushKey(DIK_A) ||
		input_->PushKey(DIK_S) ||
		input_->PushKey(DIK_D);

	// 歩きを設定する
	if (inputKey) {

		moveBehaviour_ = PlayerBehaviorType::Walk;
	} else {
		// 入力がなければ削除する
		if (CheckCurrentBehaviors({ PlayerBehaviorType::Walk })) {

			currentMoveBehaviours_.erase(PlayerBehaviorType::Walk);
		}
	}
}

void PlayerBehaviorController::MoveDash() {

	// WASDどれか押していたら
	bool inputKey =
		input_->PushKey(DIK_W) ||
		input_->PushKey(DIK_A) ||
		input_->PushKey(DIK_S) ||
		input_->PushKey(DIK_D);

	// ダッシュを設定する
	if (inputKey) {
		// 右クリックでダッシュ
		if (input_->PushMouseRight()) {

			moveBehaviour_ = PlayerBehaviorType::Dash;
		} else {
			// ダッシュ中に右クリックを離したらダッシュを止める
			if (CheckCurrentBehaviors({ PlayerBehaviorType::Dash })) {

				currentMoveBehaviours_.erase(PlayerBehaviorType::Dash);
			}
		}
	}
}

void PlayerBehaviorController::CheckWait() {

	// 入力が何もなければ
	bool inputKey =
		!input_->PushKey(DIK_W) &&
		!input_->PushKey(DIK_A) &&
		!input_->PushKey(DIK_S) &&
		!input_->PushKey(DIK_D);

	// 待ちモーションを開始させる
	if (inputKey) {

		moveBehaviour_ = PlayerBehaviorType::Wait;
	} else {
		// 入力が何かあれば削除する
		if (CheckCurrentBehaviors({ PlayerBehaviorType::Wait })) {

			currentMoveBehaviours_.erase(PlayerBehaviorType::Wait);
		}
	}
}

void PlayerBehaviorController::BehaviourRequest() {

	// 行動が追加されていれば設定する
	if (moveBehaviour_.has_value()) {

		// 同じ値はセットできないようにする
		if (!CheckCurrentBehaviors({ *moveBehaviour_ })) {

			currentMoveBehaviours_.insert(*moveBehaviour_);
		}
		// behaviourリセット
		moveBehaviour_ = std::nullopt;
	}
}

void PlayerBehaviorController::ImGui() {

	ImGui::SeparatorText("BehaviorSet");

	// 止まっている状態から攻撃...1段目
	if (ImGui::Button("Attack_1st", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::Attack_1st;
	}

	// ダッシュ攻撃...1段目
	if (ImGui::Button("DashAttack", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::DashAttack;
	}

	// 攻撃2段目
	if (ImGui::Button("Attack_2nd", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::Attack_2nd;
	}

	// 攻撃3段目
	if (ImGui::Button("Attack_3rd", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::Attack_3rd;
	}

	// 攻撃受け流し
	if (ImGui::Button("Parry", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::Parry;
	}
}

bool PlayerBehaviorController::CheckCurrentBehaviors(
	const std::initializer_list<PlayerBehaviorType> behaviours) {

	for (auto state : behaviours) {
		if (currentMoveBehaviours_.find(state) == currentMoveBehaviours_.end()) {
			return false;
		}
	}
	return true;
}

const std::unordered_set<PlayerBehaviorType>& PlayerBehaviorController::GetCurrentBehaviours() const {

	return currentMoveBehaviours_;
}