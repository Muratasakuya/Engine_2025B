#include "GameEntity3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Core/ECSManager.h>

//============================================================================
//	GameEntity3D classMethods
//============================================================================

void GameEntity3D::Init(const std::string& modelName, const std::string& name,
	const std::string& groupName, const std::optional<std::string>& animationName) {

	// entity作成
	entityId_ = ecsManager_->CreateObject3D(modelName, name, groupName, animationName);

	// component取得
	transform_ = ecsManager_->GetComponent<Transform3DComponent>(entityId_);
	materials_ = ecsManager_->GetComponent<MaterialComponent, true>(entityId_);

	// animationが存在する場合のみ処理する
	if (animationName.has_value()) {

		animation_ = ecsManager_->GetComponent<AnimationComponent>(entityId_);
	}
}

void GameEntity3D::SetColor(const Color& color, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].material.color = color;
	} else {

		for (auto& material : *materials_) {

			material.material.color = color;
		}
	}
}

void GameEntity3D::SetAlpha(float alpha, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].material.color.a = alpha;
	} else {

		for (auto& material : *materials_) {

			material.material.color.a = alpha;
		}
	}
}

void GameEntity3D::SetTextureName(const std::string& textureName, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].SetTextureName(textureName);
	} else {

		for (auto& material : *materials_) {

			material.SetTextureName(textureName);
		}
	}
}