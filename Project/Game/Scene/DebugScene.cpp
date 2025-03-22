#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Component/Manager/ComponentManager.h>
#include <Game/Camera/Manager/CameraManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	DebugScene classMethods
//============================================================================

void DebugScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager
) {

	//========================================================================
	//	load
	//========================================================================

	asset->LoadTexture("uvChecker");
	asset->LoadTexture("white");

	asset->LoadModel("teapot");
	asset->LoadModel("plane");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	// 3D
	ComponentManager::GetInstance()->CreateObject3D(
		"teapot", "playerHead", "Player", "teapotInstancing");
	ComponentManager::GetInstance()->CreateObject3D(
		"teapot", "playerBody", "Player", "teapotInstancing");

	ComponentManager::GetInstance()->CreateObject3D(
		"teapot", "enemyHead", "Enemy", "teapotInstancing");
	ComponentManager::GetInstance()->CreateObject3D(
		"teapot", "enemyBody", "Enemy", "teapotInstancing");

	ComponentManager::GetInstance()->CreateObject3D(
		"plane", "field");

	// 2D
	ComponentManager::GetInstance()->CreateObject2D(
		"white", "background");
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();
}