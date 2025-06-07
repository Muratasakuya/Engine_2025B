#include "GameEntityEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Core/ECS/Core/ECSManager.h>

//============================================================================
//	GameEntityEditor classMethods
//============================================================================

void GameEntityEditor::Init(Asset* asset) {

	// layout
	leftChildSize_ = ImVec2(320.0f, 320.0f);
	rightChildSize_ = ImVec2(384.0f, 320.0f);
	addButtonSize_ = ImVec2(208.0f, 30.0f);
	dropSize_ = ImVec2(208.0f, 30.0f);

	ecsManager_ = nullptr;
	ecsManager_ = ECSManager::GetInstance();

	asset_ = nullptr;
	asset_ = asset;
}

void GameEntityEditor::Update() {

	// 全てのentityを更新
	UpdateEntities();
}

void GameEntityEditor::ImGui() {

	EditLayout();

	// layout
	ImGui::BeginGroup();

	// 追加処理
	ImGui::BeginChild("AddChild##GameEntityEditor", leftChildSize_, true);
	ImGui::SeparatorText("Add Entity");

	AddEntity();
	ImGui::EndChild();

	// 横並びにする
	ImGui::SameLine();

	// 選択処理
	ImGui::BeginChild("SelectChild##GameEntityEditor", rightChildSize_, true);
	ImGui::SeparatorText("Select Entity");

	SelectEntity();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void GameEntityEditor::EditLayout() {

	ImGui::Begin("GameEntityEditor_EditLayout");

	ImGui::DragFloat2("leftChildSize", &leftChildSize_.x, 0.5f);
	ImGui::DragFloat2("rightChildSize", &rightChildSize_.x, 0.5f);
	ImGui::DragFloat2("addButtonSize", &addButtonSize_.x, 0.5f);

	ImGui::End();
}

void GameEntityEditor::UpdateEntities() {

	// entityが存在しなければ処理しない
	if (entities_.empty()) {
		return;
	}

	for (const auto& entities : std::views::values(entities_)) {
		for (const auto& entity : entities) {

			entity->Update();
		}
	}
}

void GameEntityEditor::AddEntity() {

	ImGui::PushID("AddEntityRow");

	if (ImGui::InputTextWithHint("##EntityName", "EntityName",
		addNameInputText_.nameBuffer, IM_ARRAYSIZE(addNameInputText_.nameBuffer))) {

		// 名前を保持
		addNameInputText_.name = addNameInputText_.nameBuffer;
	}

	// modelの名前
	DropFile("Drop model", addModelName_);
	// animationの名前
	DropFile("Drop animation", addAnimationName_);
	// typeの選択
	SelectEntityClassType(addClassType_);

	if (ImGui::Button("Add", addButtonSize_)) {

		// 入力がないときは追加不可
		if (addModelName_.has_value() &&
			!addNameInputText_.name.empty()) {

			// animationを読み込んでおく
			if (addAnimationName_.has_value()) {

				asset_->LoadAnimation(addAnimationName_.value(), addModelName_.value());
			}

			// entity作成
			std::unique_ptr<GameEntity3D>& enity = entities_[addClassType_].emplace_back(std::make_unique<GameEntity3D>());
			enity->Init(addModelName_.value(), addNameInputText_.name,
				"GameEntityEditor", addAnimationName_);

			// emitterの名前追加
			entityNames_.emplace_back(addNameInputText_.name);
		}
	}

	ImGui::PopID();
}

void GameEntityEditor::SelectEntity() {

	if (entityNames_.empty()) {
		ImGui::TextDisabled("Entity empty...");
		return;
	}

	for (int i = 0; i < static_cast<int>(entityNames_.size()); ++i) {

		const bool selected = (currentSelectIndex_ == i);
		if (ImGui::Selectable(entityNames_[i].c_str(), selected)) {

			// indexで名前を選択し設定
			currentSelectIndex_ = i;
			selectEntityName_ = entityNames_[i];
		}
		if (selected) {
			ImGui::SetItemDefaultFocus();
		}
	}
}

void GameEntityEditor::DropFile(const std::string& label, std::optional<std::string>& recieveName) {

	// ドロップ処理
	// まだ設定されていないときのみ
	if (!recieveName.has_value()) {

		// ドラッグアンドドロップで取得
		ImGui::Button(label.c_str(), dropSize_);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

				auto* payloadModelData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
				// model以外は受け付けない
				if (payloadModelData->type == AssetEditor::PendingType::Model) {

					// 名前を保存
					recieveName = std::string(payloadModelData->name);
				}
			}
			ImGui::EndDragDropTarget();
		}
	} else {

		if (ImGui::Button(("Reset##" + label).c_str(), ImVec2(addButtonSize_.x / 2.0f, addButtonSize_.y))) {

			// 名前をリセットする
			recieveName = std::nullopt;
		}
		ImGui::SameLine();

		if (recieveName.has_value()) {

			// 名を表示
			ImGui::Text((":" + recieveName.value()).c_str());
		}
	}
}

void GameEntityEditor::SelectEntityClassType(EntityClassType& classType) {

	const char* typeOptions[] = {
			"None",
	};

	int typeIndex = static_cast<int>(classType);
	if (ImGui::BeginCombo("Type", typeOptions[typeIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(typeOptions); i++) {

			const bool isSelected = (typeIndex == i);
			if (ImGui::Selectable(typeOptions[i], isSelected)) {

				typeIndex = i;
				classType = static_cast<EntityClassType>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void GameEntityEditor::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}