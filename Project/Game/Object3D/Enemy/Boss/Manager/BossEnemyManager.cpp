#include "BossEnemyManager.h"

//============================================================================
//	BossEnemyManager classMethods
//============================================================================

void BossEnemyManager::InitEntities() {

	bossEnemy_ = std::make_unique<BossEnemy>();
	bossEnemy_->Init("bossEnemy", "bossEnemy", "Enemy", "bossEnemy_idle");

	bossEnemyWeapon_ = std::make_unique<BossEnemyWeapon>();
	bossEnemyWeapon_->Init("bossEnemyWeapon", "bossEnemyWeapon", "Enemy");

	// 武器を右手を親として動かす
	if (const auto& hand = bossEnemy_->GetJointTransform("rightHand")) {

		bossEnemyWeapon_->SetParent(*hand);
	}
}

void BossEnemyManager::Init() {

	// entity初期化
	InitEntities();
}

void BossEnemyManager::Update() {

	// entity更新
	UpdateEntities();
}

void BossEnemyManager::UpdateEntities() {

	bossEnemy_->Update();
	bossEnemyWeapon_->Update();
}

void BossEnemyManager::ImGui() {


}
