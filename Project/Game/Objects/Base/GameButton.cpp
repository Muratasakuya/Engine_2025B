#include "GameButton.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionGeometry.h>
#include <Engine/Input/Input.h>
#include <Engine/Utility/EnumAdapter.h>

//============================================================================
//	GameButton classMethods
//============================================================================

void GameButton::RegisterUpdater(GameButtonResponseType type,
	std::unique_ptr<IGameButtonResponseUpdater> updater) {

	responseUpdaters_[type] = std::move(updater);
}

void GameButton::Init(const std::string& textureName, const std::string& groupName) {

	// object初期化
	GameObject2D::Init(textureName, textureName, groupName);

	// 初期値設定
	collisionSize_ = transform_->size;
}

void GameButton::Update() {

	// 衝突タイプ別に処理
	DetectCollision();
	// レスポンス更新
	UpdateResponses();
}

void GameButton::UpdateResponses() {

	GameButtonFrameContext context{};
	context.hoverAtRelease = hoverAtRelease_;

	for (auto& [type, updater] : responseUpdaters_) {

		bool prev = preActive_[type];
		bool current = currentActive_[type];

		updater->SetFrameContext(context);

		// レスポンスを更新
		if (!prev && current) {

			inactiveRunning_[type] = false;
			updater->Begin(*this);
		}
		if (prev && !current) {

			inactiveRunning_[type] = true;
		}
		if (current) {

			// アクティブ中
			updater->ActiveUpdate(*this);
		} else {
			if (inactiveRunning_[type]) {

				// 非アクティブ中
				updater->InactiveUpdate(*this);
				if (updater->IsInactiveFinished()) {

					inactiveRunning_[type] = false;
					updater->End(*this);
				}
			} else {

				updater->IdleUpdate(*this);
			}
		}
		preActive_[type] = current;
	}
}

void GameButton::DetectCollision() {

	// 判定を取らない場合は処理しない
	if (!checkCollisionEnable_) {
		return;
	}

	switch (collisionType_) {
	case GameButtonCollisionType::Mouse: {

		// 入力状態の取得
		DetectMouseCollision();
		EvaluateAnyMouseClick();
		break;
	}
	case GameButtonCollisionType::Key: {
		break;
	}
	case GameButtonCollisionType::GamePad: {
		break;
	}
	}
}

void GameButton::DetectMouseCollision() {

	// 判定
	hoverNow_ = Collision::RectToMouse(transform_->translation,
		collisionSize_, transform_->anchorPoint);

	// 入力結果
	Input* input = Input::GetInstance();
	triggerNow_ = hoverNow_ && input->TriggerMouse(mouseButton_);
	pushNow_ = input->PushMouse(mouseButton_);
	releaseNow_ = input->ReleaseMouse(mouseButton_);

	// 入力結果を設定
	currentActive_[GameButtonResponseType::OnMouse] = hoverNow_;
	currentActive_[GameButtonResponseType::MouseClick] = triggerNow_;
	currentActive_[GameButtonResponseType::OnMouseClick] = hoverNow_ && pushNow_;
}

void GameButton::EvaluateAnyMouseClick() {

	// ホバー中に入力を開始したかどうか
	if (!anyPressActive_ && pushNow_) {

		anyPressActive_ = true;
		hoverAtRelease_ = false;
		anyPressStartedOnHover_ = hoverNow_;
	}

	// 先にリリース処理を適用して状態を更新
	if (anyPressActive_ && releaseNow_) {

		hoverAtRelease_ = anyPressStartedOnHover_ && hoverNow_;
		anyPressActive_ = false;
	}

	// 入力中かどうか
	bool activeThisFrame = anyPressActive_ && anyPressStartedOnHover_;
	if (!anyPressActive_) {

		anyPressStartedOnHover_ = false;
	}

	currentActive_[GameButtonResponseType::AnyMouseClick] = activeThisFrame;
}

bool GameButton::GetHoverAtRelease() const {

	return releaseNow_ && hoverAtRelease_;
}

void GameButton::ImGui() {

	ImGui::SeparatorText("Config");

	ImGui::Checkbox("checkCollisionEnable", &checkCollisionEnable_);

	EnumAdapter<GameButtonCollisionType>::Combo("collisionType", &collisionType_);
	EnumAdapter<GameButtonResponseType>::Combo("responseType", &responseType_);
	EnumAdapter<MouseButton>::Combo("mouseButton", &mouseButton_);

	ImGui::SeparatorText("Parameters");

	ImGui::DragFloat2("collisionSize", &collisionSize_.x, 0.1f);

	ImGui::SeparatorText("Updaters");

	if (responseUpdaters_.empty()) {
		return;
	}

	for (const auto& updater : std::views::values(responseUpdaters_)) {

		updater->ImGui();
	}
}

void GameButton::FromJson(const Json& data) {

	checkCollisionEnable_ = data["checkCollisionEnable_"];
	collisionSize_ = collisionSize_.FromJson(data["collisionSize_"]);

	const auto& collisionType = EnumAdapter<GameButtonCollisionType>::FromString(data["collisionType_"]);
	collisionType_ = collisionType.value();

	const auto& responseType = EnumAdapter<GameButtonResponseType>::FromString(data["responseType_"]);
	responseType_ = responseType.value();

	const auto& mouseButton = EnumAdapter<MouseButton>::FromString(data["mouseButton_"]);
	mouseButton_ = mouseButton.value();

	for (const auto& [type, updater] : responseUpdaters_) {

		updater->FromJson(data[EnumAdapter<GameButtonResponseType>::ToString(type)]);
	}
}

void GameButton::ToJson(Json& data) {

	data["checkCollisionEnable_"] = checkCollisionEnable_;
	data["collisionSize_"] = collisionSize_.ToJson();

	data["collisionType_"] = EnumAdapter<GameButtonCollisionType>::ToString(collisionType_);
	data["responseType_"] = EnumAdapter<GameButtonResponseType>::ToString(responseType_);
	data["mouseButton_"] = EnumAdapter<MouseButton>::ToString(mouseButton_);

	for (const auto& [type, updater] : responseUpdaters_) {

		updater->ToJson(data[EnumAdapter<GameButtonResponseType>::ToString(type)]);
	}
}