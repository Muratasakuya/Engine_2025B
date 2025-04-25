#include "PlayerPartsController.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerPartsController classMethods
//============================================================================

void PlayerPartsController::InitParts(FollowCamera* followCamera) {

	// 体
	body_ = std::make_unique<PlayerBody>();
	body_->Init();
	// followCameraを設定する
	body_->SetFollowCamera(followCamera);

	// 手
	// 右
	rightHand_ = std::make_unique<PlayerRightHand>();
	rightHand_->Init();
	// 左
	leftHand_ = std::make_unique<PlayerLeftHand>();
	leftHand_->Init();

	// 剣
	// 右
	rightSword_ = std::make_unique<PlayerRightSword>();
	rightSword_->Init();
	// 左
	leftSword_ = std::make_unique<PlayerLeftSword>();
	leftSword_->Init();

	// 手に親を設定する
	rightHand_->SetParent(body_->GetTransform());
	leftHand_->SetParent(body_->GetTransform());
	// 剣に親を設定する
	rightSword_->SetParent(rightHand_->GetTransform());
	leftSword_->SetParent(leftHand_->GetTransform());
}

void PlayerPartsController::SetParam() {

	// 値を設定
	rightHand_->SetParam(rightHandParam_);
	leftHand_->SetParam(leftHandParam_);
	rightSword_->SetParam(rightSwordParam_);
	leftSword_->SetParam(leftSwordParam_);
}

void PlayerPartsController::Init(FollowCamera* followCamera) {

	// 各partsの初期化
	InitParts(followCamera);

	// json適応
	ApplyJson();

	// 各partsに値を設定
	SetParam();
}

void PlayerPartsController::Update(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	// 設定されたbehaviorで更新
	UpdateBehavior(behaviors);
}

void PlayerPartsController::UpdateBehavior(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	// 歩き処理
	body_->UpdateWalk();

	// ダッシュ
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Dash })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::Dash); });
	}
	// 止まっている状態から攻撃...1段目
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::NormalAttack })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::NormalAttack); });
	}
	// ダッシュ攻撃...1段目
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::DashAttack })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::DashAttack); });
	}
	// 攻撃2段目
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Attack2nd })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::Attack2nd); });
	}
	// 攻撃3段目
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Attack3rd })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::Attack3rd); });
	}
	// 攻撃受け流し
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Parry })) {

		ForEachParts([](BasePlayerParts* part) {
			part->ExecuteBehavior(PlayerBehaviorType::Parry); });
	}
	// 体の向きを移動に合わせる
	body_->RotateToDirection();
}

void PlayerPartsController::ImGui() {

	if (ImGui::Button("Save Json", ImVec2(itemWidth_, 32.0f))) {

		SaveJson();
	}

	if (ImGui::BeginTabBar("PlayerPartsControllerTabs")) {

		if (ImGui::BeginTabItem("Body")) {

			body_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("RightHand")) {

			rightHandParam_.ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("LeftHand")) {

			leftHandParam_.ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("RightSword")) {

			rightSwordParam_.ImGui();
			rightSword_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("LeftSword")) {

			leftSwordParam_.ImGui();
			leftSword_->ImGui();
			ImGui::EndTabItem();
		}

		SetParam();

		ImGui::EndTabBar();
	}
}

void PlayerPartsController::ApplyJson() {

	// 名前を設定
	rightHandParam_.name = "rightHandParam";
	leftHandParam_.name = "leftHandParam";
	rightSwordParam_.name = "rightSwordParam";
	leftSwordParam_.name = "leftSwordParam";

	// 各parameterの値を適応
	rightHandParam_.ApplyJson();
	leftHandParam_.ApplyJson();
	rightSwordParam_.ApplyJson();
	leftSwordParam_.ApplyJson();
}

void PlayerPartsController::SaveJson() {

	// 各parameterの値を保存
	rightHandParam_.SaveJson();
	leftHandParam_.SaveJson();
	rightSwordParam_.SaveJson();
	leftSwordParam_.SaveJson();

	// 各クラスごと
	body_->SaveJson();
	rightSword_->SaveJson();
	leftSword_->SaveJson();
}

bool PlayerPartsController::CheckCurrentBehaviors(const std::unordered_set<PlayerBehaviorType>& currentBehaviors,
	const std::initializer_list<PlayerBehaviorType> behaviours) {

	for (auto state : behaviours) {
		if (currentBehaviors.find(state) == currentBehaviors.end()) {
			return false;
		}
	}
	return true;
}