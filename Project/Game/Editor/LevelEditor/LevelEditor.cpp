#include "LevelEditor.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	LevelEditor classMethods
//============================================================================

void LevelEditor::Init() {

	sceneBuilder_ = std::make_unique<SceneBuilder>();
	sceneBuilder_->Init();
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
}