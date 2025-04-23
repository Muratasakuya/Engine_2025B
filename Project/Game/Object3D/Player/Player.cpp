#include "Player.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	Player classMethods
//============================================================================

void Player::Init() {

	// 各partsの初期化
	partsController_ = std::make_unique<PlayerPartsController>();
	partsController_->Init();
}

void Player::Update() {

	// 各partsの更新
	partsController_->Update();
}