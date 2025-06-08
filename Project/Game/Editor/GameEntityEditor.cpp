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

	ImGui::BeginChild("EditChild##GameEntityEditor");
	ImGui::SeparatorText("Edit Entity");

	// 削除処理
	RemoveEntity();

	// 操作処理
	EditEntity();
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

			// 実際に追加された名前を取得
			const std::string name = enity->GetTag().name;

			// emitterの名前追加
			entityNames_.emplace_back(name);
			entityHandles_.push_back(EntityHandle(addClassType_, entities_[addClassType_].size() - 1));
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

		const bool selected = (currentSelectIndex_.has_value() && currentSelectIndex_.value() == i);
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

void GameEntityEditor::EditEntity() {

	// 選択されていないときは何もしない
	if (!currentSelectIndex_.has_value()) {
		return;
	}

	// entity操作
	const EntityHandle& handle = entityHandles_[currentSelectIndex_.value()];
	GameEntity3D* entity = entities_.at(handle.classType)[handle.innerIndex].get();
	entity->ImGui();
}

void GameEntityEditor::RemoveEntity() {

	// 選択されていないときは何もしない
	if (!currentSelectIndex_.has_value()) {
		return;
	}

	if (ImGui::Button("Remove Entity", addButtonSize_)) {

		// 現在のインデックスを取得
		const int listIndex = currentSelectIndex_.value();
		const EntityHandle handle = entityHandles_[listIndex];

		// map内の選択されているentityを削除する
		auto mapIt = entities_.find(handle.classType);
		if (mapIt != entities_.end() && handle.innerIndex < mapIt->second.size()) {

			mapIt->second.erase(mapIt->second.begin() + static_cast<std::ptrdiff_t>(handle.innerIndex));

			// innerIndexも削除する
			for (auto& h : entityHandles_) {
				if (h.classType == handle.classType && h.innerIndex > handle.innerIndex) {
					--h.innerIndex;
				}
			}
		}

		entityNames_.erase(entityNames_.begin() + listIndex);
		entityHandles_.erase(entityHandles_.begin() + listIndex);

		if (entityNames_.empty()) {

			currentSelectIndex_.reset();
			selectEntityName_.reset();
		} else {

			const int newIndex = std::min(listIndex, static_cast<int>(entityNames_.size() - 1));
			currentSelectIndex_ = newIndex;
			selectEntityName_ = entityNames_[newIndex];
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