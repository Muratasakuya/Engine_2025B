#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] LightManager* lightManager,
	[[maybe_unused]] PostProcessSystem* postProcessSystem
) {

	//========================================================================
	//	load
	//========================================================================

	asset->LoadTexture("uvChecker");
	asset->LoadTexture("circle");
	asset->LoadTexture("sirialHitEffect");
	asset->LoadTexture("effectCircle");

	// normalMap
	asset->LoadTexture("Bricks_Color");
	asset->LoadTexture("Bricks_NormalDX");

	asset->LoadModel("stageField");
	asset->LoadModel("teapot");
	asset->LoadModel("plane");
	asset->LoadModel("multiMaterial");
	asset->LoadModel("suzanne");
	asset->LoadModel("bricks");
	asset->LoadModel("sponza");

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessSystem_ = postProcessSystem;

	// bloom処理
	postProcessSystem_->Create({ PostProcess::Bloom });
	postProcessSystem_->AddProcess(PostProcess::Bloom);

	//========================================================================
	//	sceneObject
	//========================================================================

	// camera
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	cameraManager->SetCamera(gameCamera_.get());

	// light
	gameLight_ = std::make_unique<PunctualLight>();
	gameLight_->Init();

	lightManager->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	GameObjectHelper::CreateObject3D("sponza", "sponza", "Sponza");
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	gameCamera_->Update();
}