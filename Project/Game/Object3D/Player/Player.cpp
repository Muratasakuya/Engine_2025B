#include "Player.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	Player classMethods
//============================================================================

void Player::Init(FollowCamera* followCamera) {

	// 各partsの初期化
	partsController_ = std::make_unique<PlayerPartsController>();
	partsController_->Init(followCamera);

	// behaviorの初期化
	behavior_ = std::make_unique<PlayerAttackBehavior>();
	behavior_->Init();
}

void Player::Update() {

	// behaviorの更新
	behavior_->Update();

	// 各partsの更新
	partsController_->Update(behavior_->GetCurrentBehaviours());
}