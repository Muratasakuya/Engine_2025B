#include "TitleScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Input/Input.h>
#include <Game/Scene/Manager/SceneManager.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void TitleScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] LightManager* lightManager,
	[[maybe_unused]] PostProcessSystem* postProcessSystem
) {

	//========================================================================
	//	load
	//========================================================================

	asset->LoadTexture("white");

	// titleName
	asset->LoadTexture("titleName");

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

}

void TitleScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	gameCamera_->Update();
}