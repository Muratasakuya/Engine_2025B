#include "TitleController.h"

//============================================================================
//	TitleController classMethods
//============================================================================

void TitleController::Init() {

	// 表示スプライト
	displaySprite_ = std::make_unique<TitleDisplaySprite>();
	displaySprite_->Init();
}

void TitleController::Update() {

	// 各機能の更新処理
	displaySprite_->Update();
}