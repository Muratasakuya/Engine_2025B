#include "PlayerBehaviorController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	PlayerBehaviorController classMethods
//============================================================================

void PlayerBehaviorController::Init() {

	input_ = nullptr;
	input_ = Input::GetInstance();

	isAcceptedMode_ = false;

	// json適応
	ApplyJson();
}

void PlayerBehaviorController::Update() {

	// behaviorの変更不可、設定不可
	if (isAcceptedMode_) {
		return;
	}

	// 移動更新処理
	UpdateMove();
	// 攻撃更新処理
	UpdateAttack();

	// 待機モーションチェック
	CheckWait();

	// behaviourを設定する
	BehaviourRequest();
}

void PlayerBehaviorController::UpdateMove() {

	// 攻撃中はここの操作はできないようにしておく
	bool isAttack = false;
	isAttack |= limits_[PlayerBehaviorType::Attack_1st].isUpdating_;

	if (isAttack) {
		return;
	}

	// 歩き処理
	MoveWalk();
	// ダッシュ処理
	MoveDash();
}

void PlayerBehaviorController::UpdateAttack() {

	// 1段階目攻撃処理
	FirstAttack();
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
		if (CheckCurrentBehaviors({ PlayerBehaviorType::Walk }, MatchType::All)) {

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
			// ダッシュになったら歩き操作は消す
			currentMoveBehaviours_.erase(PlayerBehaviorType::Walk);
		} else {
			// ダッシュ中に右クリックを離したらダッシュを止める
			if (CheckCurrentBehaviors({ PlayerBehaviorType::Dash }, MatchType::All)) {

				currentMoveBehaviours_.erase(PlayerBehaviorType::Dash);
			}
		}
	}
}

void PlayerBehaviorController::FirstAttack() {

	// 1段目の攻撃を設定するを設定する
	// 1段目攻撃中は受け付けない
	if (!limits_[PlayerBehaviorType::Attack_1st].isUpdating_) {
		// 待機か、歩き中
		if (CheckCurrentBehaviors({
			PlayerBehaviorType::Wait,
			PlayerBehaviorType::Walk }, MatchType::Any)) {
			// 左クリックで 1段目の攻撃
			if (input_->TriggerKey(DIK_SPACE)) {

				moveBehaviour_ = PlayerBehaviorType::Attack_1st;

				// 更新状態にする
				limits_[PlayerBehaviorType::Attack_1st].isUpdating_ = true;
				// 現在有効な操作をすべて削除する
				currentMoveBehaviours_.clear();
			}
		}
	}

	// 設定されているとき
	if (CheckCurrentBehaviors({ PlayerBehaviorType::Attack_1st }, MatchType::All)) {

		// 経過時間を進める
		limits_[PlayerBehaviorType::Attack_1st].UpdateElapseTime();
		// 最大時間になったらリセットし、攻撃を止める
		if (limits_[PlayerBehaviorType::Attack_1st].isReached) {

			limits_[PlayerBehaviorType::Attack_1st].Reset();
			currentMoveBehaviours_.erase(PlayerBehaviorType::Attack_1st);
		}
	}
}

void PlayerBehaviorController::CheckWait() {

	// 攻撃中はここの操作はできないようにしておく
	bool isAttack = false;
	isAttack |= limits_[PlayerBehaviorType::Attack_1st].isUpdating_;

	if (isAttack) {
		return;
	}

	// 入力が何もないか、moveBehaviour_に何も値が入っていないか
	bool inputKey =
		!input_->PushKey(DIK_W) &&
		!input_->PushKey(DIK_A) &&
		!input_->PushKey(DIK_S) &&
		!input_->PushKey(DIK_D);

	// 待ちモーションを開始させる
	if (inputKey && !moveBehaviour_.has_value()) {

		moveBehaviour_ = PlayerBehaviorType::Wait;
	} else {
		// 何か操作があれば削除する
		if (CheckCurrentBehaviors({ PlayerBehaviorType::Wait }, MatchType::All)) {

			currentMoveBehaviours_.erase(PlayerBehaviorType::Wait);
		}
	}
}

void PlayerBehaviorController::BehaviourRequest() {

	// 行動が追加されていれば設定する
	if (moveBehaviour_.has_value()) {

		// 同じ値はセットできないようにする
		if (!CheckCurrentBehaviors({ *moveBehaviour_ }, MatchType::All)) {

			currentMoveBehaviours_.insert(*moveBehaviour_);
		}
		// behaviourリセット
		moveBehaviour_ = std::nullopt;
	}
}

void PlayerBehaviorController::ImGui() {

	ImGui::SeparatorText("BehaviorSet");

	ImGui::Text(std::format("isAcceptedMode: {}", isAcceptedMode_).c_str());

	// behaviorを更新できないようにする
	if (ImGui::Button("AcceptedMode", ImVec2(itemWidth_, 32.0f))) {

		isAcceptedMode_ = !isAcceptedMode_;
	}

	// 値を保存
	if (ImGui::Button("Save Json", ImVec2(itemWidth_, 32.0f))) {

		SaveJson();
	}

	// 現在有効なbehaviorを表示
	if (!currentMoveBehaviours_.empty()) {

		std::string behaviourText;
		for (auto behaviour : currentMoveBehaviours_) {
			switch (behaviour) {
			case PlayerBehaviorType::Wait:       behaviourText += "Wait, "; break;
			case PlayerBehaviorType::Walk:       behaviourText += "Walk, "; break;
			case PlayerBehaviorType::Dash:       behaviourText += "Dash, "; break;
			case PlayerBehaviorType::Attack_1st: behaviourText += "Attack_1st, "; break;
			case PlayerBehaviorType::DashAttack: behaviourText += "DashAttack, "; break;
			case PlayerBehaviorType::Attack_2nd: behaviourText += "Attack_2nd, "; break;
			case PlayerBehaviorType::Attack_3rd: behaviourText += "Attack_3rd, "; break;
			case PlayerBehaviorType::Parry:      behaviourText += "Parry, "; break;
			}
		}

		if (!behaviourText.empty()) {

			behaviourText.pop_back();
			behaviourText.pop_back();
		}

		ImGui::Text("%s", std::format("Current Behaviors: {}", behaviourText).c_str());
	}

	// 止まっている状態から攻撃...1段目
	if (ImGui::CollapsingHeader("Attack_1st")) {

		if (ImGui::Button("Execute##Attack_1st", ImVec2(itemWidth_, 32.0f))) {

			moveBehaviour_ = PlayerBehaviorType::Attack_1st;
		}
		limits_[PlayerBehaviorType::Attack_1st].ImGui();
	}
}

bool PlayerBehaviorController::CheckCurrentBehaviors(
	const std::initializer_list<PlayerBehaviorType> behaviours, MatchType matchType) {

	// 全部一致
	if (matchType == MatchType::All) {
		for (auto state : behaviours) {
			if (currentMoveBehaviours_.find(state) == currentMoveBehaviours_.end()) {

				// 1つもない場合
				return false;
			}
		}
		// 全部あった場合
		return true;
	}
	// どれか一致
	else if (matchType == MatchType::Any) {
		for (auto state : behaviours) {
			if (currentMoveBehaviours_.find(state) != currentMoveBehaviours_.end()) {

				// 1つが一致したとき
				return true;
			}
		}
		// 1つもない場合
		return false;
	}
	return false;
}

const std::unordered_set<PlayerBehaviorType>& PlayerBehaviorController::GetCurrentBehaviours() const {

	return currentMoveBehaviours_;
}

void PlayerBehaviorController::LimitTime::UpdateElapseTime() {

	elapsed += GameTimer::GetDeltaTime();

	// 経過時間チェック
	if (elapsed > limit) {

		// 時間を過ぎた
		isReached = true;
	}
}

void PlayerBehaviorController::LimitTime::Reset() {

	elapsed = 0.0f;
	isUpdating_ = false;
	isReached = false;
}

void PlayerBehaviorController::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/Controller/behaviorController.json", data)) {
		return;
	}

	limits_[PlayerBehaviorType::Attack_1st].ApplyJson(data, "Attack_1st");
}

void PlayerBehaviorController::SaveJson() {

	Json data;

	limits_[PlayerBehaviorType::Attack_1st].SaveJson(data, "Attack_1st");

	JsonAdapter::Save("Player/Controller/behaviorController.json", data);
}

void PlayerBehaviorController::LimitTime::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::Text(std::format("isUpdating: {}", isUpdating_).c_str());

	ImGui::DragFloat("limit", &limit, 0.01f);

	ImGui::PopItemWidth();
}

void PlayerBehaviorController::LimitTime::ApplyJson(
	const Json& data, const std::string& key) {

	limit = JsonAdapter::GetValue<float>(data[key], "limit");
}

void PlayerBehaviorController::LimitTime::SaveJson(
	Json& data, const std::string& key) {

	data[key]["limit"] = limit;
}