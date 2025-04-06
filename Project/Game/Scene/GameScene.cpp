#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/User/ComponentHelper.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] PostProcessManager* postProcessManager
) {

	//========================================================================
	//	load
	//========================================================================

	// Environment
	asset->LoadModel("stageField");

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

	stageField_ = std::make_unique<StageField>();
	stageField_->Init();
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	gameCamera_->Update();

	stageField_->Update();
}