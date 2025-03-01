#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
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

	asset->LoadModel("teapot");
	asset->LoadModel("suzanne");
	asset->LoadModel("multiMaterial");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	objects_.resize(3);
	objects_[0] = std::make_unique<BaseGameObject>();
	objects_[0]->CreateModel("teapot");

	objects_[1] = std::make_unique<BaseGameObject>();
	objects_[1]->CreateModel("multiMaterial");

	objects_[2] = std::make_unique<BaseGameObject>();
	objects_[2]->CreateModel("suzanne");
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();

	LineRenderer::GetInstance()->DrawGrid(32, 16.0f, Color::White());
}