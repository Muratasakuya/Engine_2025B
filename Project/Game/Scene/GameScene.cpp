#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Renderer/LineRenderer.h>
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

	Load(asset);

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
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	cameraManager->SetCamera(followCamera_.get());

	// light
	gameLight_ = std::make_unique<PunctualLight>();
	gameLight_->Init();

	lightManager->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	object3DEditor_ = std::make_unique<Object3DEditor>();
	object3DEditor_->Init(asset);

	player_ = std::make_unique<Player>();
	player_->Init();

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();
}

void GameScene::Load(Asset* asset) {

	// player
	asset->LoadModel("playerBody");
	asset->LoadModel("playerLeftHand");
	asset->LoadModel("playerRightHand");
	asset->LoadModel("playerSword");

	// environment
	asset->LoadModel("stageField");

	// primitive
	asset->LoadModel("primitivePlane");
}