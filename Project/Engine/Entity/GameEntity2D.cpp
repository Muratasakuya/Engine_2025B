#include "GameEntity2D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Scene/Camera/BaseCamera.h>

//============================================================================
//	GameEntity2D classMethods
//============================================================================

void GameEntity2D::Init(const std::string& textureName,
	const std::string& name, const std::string& groupName) {

	// entity作成
	entityId_ = ecsManager_->CreateObject2D(textureName, name, groupName);

	// component取得
	transform_ = ecsManager_->GetComponent<Transform2DComponent>(entityId_);
	material_ = ecsManager_->GetComponent<SpriteMaterialComponent>(entityId_);
	sprite_ = ecsManager_->GetComponent<SpriteComponent>(entityId_);
}

void GameEntity2D::SetCenterTranslation() {

	// 画面中心に設定
	transform_->translation = Vector2(Config::kWindowWidthf / 2.0f, Config::kWindowHeightf / 2.0f);
}

void GameEntity2D::ProjectToScreen(const Vector3& translation, BaseCamera* camera) {

	Matrix4x4 viewMatrix = camera->GetViewMatrix();
	Matrix4x4 projectionMatrix = camera->GetProjectionMatrix();

	Vector3 viewPos = Vector3::Transform(translation, viewMatrix);
	Vector3 clipPos = Vector3::Transform(viewPos, projectionMatrix);

	float screenX = (clipPos.x * 0.5f + 0.5f) * Config::kWindowWidthf;
	float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * Config::kWindowHeightf;

	transform_->translation = Vector2(screenX, screenY);
}