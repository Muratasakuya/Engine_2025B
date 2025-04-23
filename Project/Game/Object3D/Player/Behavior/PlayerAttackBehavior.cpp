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

	// 歩き処理
	MoveWalk();
	// ダッシュ処理
	MoveDash();

	// behaviourを設定する
	BehaviourRequest();
}

void PlayerAttackBehavior::MoveWalk() {

	// dash中は歩き処理を行わない
	if (CheckCurrentBehaviors({ PlayerBehaviorType::Dash })) {
		return;
	}

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
		// 歩き中にWASDを離したら歩きを止める
		if (CheckCurrentBehaviors({ PlayerBehaviorType::Walk })) {

			currentMoveBehaviours_.erase(PlayerBehaviorType::Walk);
		}
	}
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

			moveBehaviour_ = PlayerBehaviorType::Walk;
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