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

	asset->LoadModel("teapot");
	asset->LoadModel("plane");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	ComponentManager::GetInstance()->CreateObject3D("teapot", std::nullopt, "playerHead", "teapotInstancing");
	ComponentManager::GetInstance()->CreateObject3D("teapot", std::nullopt, "playerBody", "teapotInstancing");

	ComponentManager::GetInstance()->CreateObject3D("plane", std::nullopt, "playerRightLeg", "planeInstancing");
	ComponentManager::GetInstance()->CreateObject3D("plane", std::nullopt, "playerLeftLeg", "planeInstancing");

	ComponentManager::GetInstance()->CreateObject2D("uvChecker");
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();
}