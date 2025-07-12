#include "LevelEditor.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	LevelEditor classMethods
//============================================================================

void LevelEditor::Init(const std::string& initSceneFile) {

	sceneBuilder_ = std::make_unique<SceneBuilder>();
	sceneBuilder_->Init(jsonPath_);

	sceneBuilder_->SetFile(initSceneFile);

	// entityの作成
	BuildEntities();

	rightChildSize_ = ImVec2(384.0f, 320.0f);
	buttonSize_ = ImVec2(256.0f, 32.0f);
}

void LevelEditor::SaveEntity(GameEntity3D* entity) {

	const std::string& identifier = Algorithm::RemoveAfterUnderscore(entity->GetIdentifier());
	if (ImGui::Button(("Save Material..." + identifier + ".json").c_str())) {

		Json data;
		// materialを保存
		entity->SaveMaterial(data);

		JsonAdapter::Save(jsonPath_ + identifier + ".json", data);
	}
}

void LevelEditor::Update() {

	// entityの作成
	BuildEntities();

	// entityの更新処理
	UpdateEntities();
}

void LevelEditor::BuildEntities() {

	// sceneを構築するかどうか
	if (sceneBuilder_->IsCreate()) {

		// .jsonを基に作成
		sceneBuilder_->CreateEntitiesMap(entitiesMap_);
		// リセット
		sceneBuilder_->Reset();
	}
}

void LevelEditor::UpdateEntities() {

	if (entitiesMap_.empty()) {
		return;
	}

	// 種類ごとに全て更新
	for (const auto& entityMap : std::views::values(entitiesMap_)) {
		for (const auto& entities : entityMap) {

			entities->Update();
		}
	}
}

void LevelEditor::ImGui() {

	sceneBuilder_->ImGui();

	// 選択処理
	ImGui::BeginChild("SelectChild##SceneBuilder", rightChildSize_, true);
	ImGui::SeparatorText("Select Entity");

	SelectEntity();
	ImGui::EndChild();

	ImGui::BeginChild("EditChild##SceneBuilder");
	ImGui::SeparatorText("Edit Entity");

	// 操作処理
	EditEntity();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void LevelEditor::SelectEntity() {

	// entityType選択
	const char* typeOptions[] = { 
		"None","CrossMarkWall"
	};

	int typeIndex = static_cast<int>(currentSelectType_);
	if (ImGui::BeginCombo("Type", typeOptions[typeIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(typeOptions); ++i) {

			bool isSelected = (typeIndex == i);
			if (ImGui::Selectable(typeOptions[i], isSelected)) {

				typeIndex = i;
				currentSelectType_ = static_cast<Level::EntityType>(i);
				currentSelectIndex_.reset();
				selectFilter_.Clear();
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	auto mapIt = entitiesMap_.find(currentSelectType_);
	if (mapIt == entitiesMap_.end() || mapIt->second.empty()) {
		ImGui::TextDisabled("No Entity for this type...");
		return;
	}

	// 検索ボックス 
	selectFilter_.Draw("##SearchEntity", rightChildSize_.x - 16.0f);
	ImGui::Separator();

	// 一覧
	const auto& entityVec = mapIt->second;
	bool hasResult = false;
	for (int i = 0; i < static_cast<int>(entityVec.size()); ++i) {

		const std::string name = entityVec[i]->GetTag().name + "_" + entityVec[i]->GetIdentifier();
		if (!selectFilter_.PassFilter(name.c_str())) { 
			continue;
		}

		hasResult = true;
		bool selected = (currentSelectIndex_ && *currentSelectIndex_ == i);
		if (ImGui::Selectable(name.c_str(), selected)) {

			currentSelectIndex_ = i;
		}
		if (selected) ImGui::SetItemDefaultFocus();
	}

	// 検索に引っかからなかった場合
	if (!hasResult) {

		ImGui::TextDisabled("No matches...");
	}
}

void LevelEditor::EditEntity() {

	if (!currentSelectIndex_.has_value()) {
		return;
	}

	auto mapIt = entitiesMap_.find(currentSelectType_);
	if (mapIt == entitiesMap_.end()) {
		return;
	}

	const auto& entityVec = mapIt->second;
	int index = currentSelectIndex_.value();
	if (index < 0 || index >= static_cast<int>(entityVec.size())) {
		return;
	}

	SaveEntity(entityVec[index].get());
	entityVec[index]->ImGui();
}