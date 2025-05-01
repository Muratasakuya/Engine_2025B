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
	body_->Init(followCamera);

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

void PlayerPartsController::Init(FollowCamera* followCamera) {

	// 各partsの初期化
	InitParts(followCamera);

	// json適応
	ApplyJson();
}

void PlayerPartsController::Update(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	// 設定されたbehaviorで更新
	UpdateBehavior(behaviors);
}

void PlayerPartsController::UpdateBehavior(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	struct BehaviorInfo {

		PlayerBehaviorType type;
		bool needReset;
	};
	// 更新対象一覧
	const std::vector<BehaviorInfo> behaviorList = {

		{ PlayerBehaviorType::Wait, true },
		{ PlayerBehaviorType::Walk, true },
		{ PlayerBehaviorType::Dash, true },
		{ PlayerBehaviorType::Attack_1st, true },
		{ PlayerBehaviorType::DashAttack, false },
		{ PlayerBehaviorType::Attack_2nd, true },
		{ PlayerBehaviorType::Attack_3rd, true },
		{ PlayerBehaviorType::Parry, false },
	};

	for (const auto& behaviorInfo : behaviorList) {

		// 有効なbehaviorを更新
		if (CheckCurrentBehaviors(behaviors, { behaviorInfo.type })) {

			ForEachParts([&](BasePlayerParts* part) {
				part->ExecuteBehavior(behaviorInfo.type);
				});
		}
		// リセットが必要なら行う
		else if (behaviorInfo.needReset) {

			ForEachParts([&](BasePlayerParts* part) {
				part->ResetBehavior(behaviorInfo.type);
				});
		}
	}
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

			rightHand_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("LeftHand")) {

			leftHand_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("RightSword")) {

			rightSword_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("LeftSword")) {

			leftSword_->ImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void PlayerPartsController::ApplyJson() {
}

void PlayerPartsController::SaveJson() {

	// 各クラスごと
	body_->SaveJson();
	rightHand_->SaveJson();
	leftHand_->SaveJson();
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