#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/User/ComponentHelper.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessManager.h>
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
	asset->LoadModel("teapot");
	asset->LoadModel("multiMaterial");
	asset->LoadModel("suzanne");

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessManager_ = postProcessManager;

	// bloom処理
	postProcessManager_->Create({ PostProcess::Bloom });
	postProcessManager_->AddProcess(PostProcess::Bloom);

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

	object3DEditor_ = std::make_unique<Object3DEditor>();
	object3DEditor_->Init(asset);

	stageField_ = std::make_unique<StageField>();
	stageField_->Init();
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	gameCamera_->Update();
}