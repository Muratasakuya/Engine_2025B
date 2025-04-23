#include "PlayerPartsController.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerPartsController classMethods
//============================================================================

void PlayerPartsController::InitParts() {

	// 体
	body_ = std::make_unique<PlayerBody>();
	body_->Init();

	// 手
	// 右
	rightHand_ = std::make_unique<PlayerRightHand>();
	rightHand_->Init();
	// 左
	leftHand_ = std::make_unique<PlayerLeftHand>();
	leftHand_->Init();

	// 剣
	sword_ = std::make_unique<PlayerSword>();
	sword_->Init();

	// 手に親を設定する
	rightHand_->SetParent(body_->GetTransform());
	leftHand_->SetParent(body_->GetTransform());
	// 剣に親を設定する
	sword_->SetParent(rightHand_->GetTransform());
}

void PlayerPartsController::Init() {

	// 各partsの初期化
	InitParts();

	// json適応
	ApplyJson();
}

void PlayerPartsController::Update() {

	// 各partsに値を設定して更新
	SetUpdateParam();
}

void PlayerPartsController::SetUpdateParam() {

	// 値を設定
	rightHand_->SetParam(rightHandParam_);
	leftHand_->SetParam(leftHandParam_);
	sword_->SetParam(swordParam_);
}

void PlayerPartsController::ImGui() {

	if (ImGui::Button("Save Json", ImVec2(itemWidth_, 32.0f))) {

		SaveJson();
	}

	if (ImGui::BeginTabBar("PlayerPartsControllerTabs")) {

		if (ImGui::BeginTabItem("RightHand")) {

			rightHandParam_.ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("LeftHand")) {

			leftHandParam_.ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sword")) {

			swordParam_.ImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PlayerPartsController::ApplyJson() {

	// 名前を設定
	rightHandParam_.name = "rightHandParam";
	leftHandParam_.name = "leftHandParam";
	swordParam_.name = "swordParam";

	// 各parameterの値を適応
	rightHandParam_.ApplyJson();
	leftHandParam_.ApplyJson();
	swordParam_.ApplyJson();
}

void PlayerPartsController::SaveJson() {

	// 各parameterの値を保存
	rightHandParam_.SaveJson();
	leftHandParam_.SaveJson();
	swordParam_.SaveJson();
}