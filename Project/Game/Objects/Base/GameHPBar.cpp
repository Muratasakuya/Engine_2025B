#include "GameHPBar.h"

//============================================================================
//	GameHPBar classMethods
//============================================================================

void GameHPBar::Init(const std::string& textureName, const std::string& alphaTextureName,
	const std::string& name, const std::string& groupName) {

	// entity初期化
	GameEntity2D::Init(textureName, name, groupName);

	// alphaTexture設定
	sprite_->SetAlphaTextureName(alphaTextureName);
	material_->material.useAlphaColor = true;
}

void GameHPBar::Update(int currentHP, int maxHP) {

	if (maxHP <= 0) {
		return;
	}

	// HPの割合を計算
	material_->material.alphaReference = std::clamp(
		1.0f - static_cast<float>(currentHP) / static_cast<float>(maxHP), 0.0f, 1.0f);
}