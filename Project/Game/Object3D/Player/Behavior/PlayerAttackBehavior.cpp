#include "PlayerAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerAttackBehavior classMethods
//============================================================================

void PlayerAttackBehavior::Init() {

	input_ = nullptr;
	input_ = Input::GetInstance();
}

void PlayerAttackBehavior::Update() {

	// ダッシュ処理
	MoveDash();

	// behaviourを設定する
	BehaviourRequest();
}

void PlayerAttackBehavior::MoveDash() {

	// WASDどれか押していたら
	bool inputKey =
		input_->PushKey(DIK_W) ||
		input_->PushKey(DIK_A) ||
		input_->PushKey(DIK_S) ||
		input_->PushKey(DIK_D);

	// ダッシュを設定する
	if (inputKey) {
		// 右クリックでダッシュ
		if (input_->PushMouseLeft()) {

			moveBehaviour_ = PlayerBehaviorType::Dash;
		} else {
			// ダッシュ中に右クリックを離したらダッシュを止める
			if (CheckCurrentBehaviors({ PlayerBehaviorType::Dash })) {

				currentMoveBehaviours_.erase(PlayerBehaviorType::Dash);
			}
		}
	}
}

void PlayerAttackBehavior::BehaviourRequest() {

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

void PlayerAttackBehavior::ImGui() {

	ImGui::SeparatorText("BehaviorSet");

	if (ImGui::Button("Parry", ImVec2(itemWidth_, 32.0f))) {

		moveBehaviour_ = PlayerBehaviorType::Parry;
	}
}

bool PlayerAttackBehavior::CheckCurrentBehaviors(
	const std::initializer_list<PlayerBehaviorType> behaviours) {

	for (auto state : behaviours) {
		if (currentMoveBehaviours_.find(state) == currentMoveBehaviours_.end()) {
			return false;
		}
	}
	return true;
}

const std::unordered_set<PlayerBehaviorType>& PlayerAttackBehavior::GetCurrentBehaviours() const {

	return currentMoveBehaviours_;
}