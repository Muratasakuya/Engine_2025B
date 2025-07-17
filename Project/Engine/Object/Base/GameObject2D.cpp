#include "GameObject2D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Scene/Camera/BaseCamera.h>

//============================================================================
//	GameObject2D classMethods
//============================================================================

void GameObject2D::Init(const std::string& textureName,
	const std::string& name, const std::string& groupName) {

	// object作成
	objectId_ = objectManager_->CreateObject2D(textureName, name, groupName);

	// data取得
	transform_ = objectManager_->GetData<Transform2D>(objectId_);
	material_ = objectManager_->GetData<SpriteMaterial>(objectId_);
	sprite_ = objectManager_->GetData<Sprite>(objectId_);
}

void GameObject2D::SetCenterTranslation() {

	// 画面中心に設定
	transform_->translation = Vector2(Config::kWindowWidthf / 2.0f, Config::kWindowHeightf / 2.0f);
}

void GameObject2D::ProjectToScreen(const Vector3& translation, const BaseCamera& camera) {

	Matrix4x4 viewMatrix = camera.GetViewMatrix();
	Matrix4x4 projectionMatrix = camera.GetProjectionMatrix();

	Vector3 viewPos = Vector3::Transform(translation, viewMatrix);
	Vector3 clipPos = Vector3::Transform(viewPos, projectionMatrix);

	float screenX = (clipPos.x * 0.5f + 0.5f) * Config::kWindowWidthf;
	float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * Config::kWindowHeightf;

	transform_->translation = Vector2(screenX, screenY);
}