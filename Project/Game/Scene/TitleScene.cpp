#include "TitleScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Input/Input.h>
#include <Game/Camera/Manager/CameraManager.h>
#include <Game/Scene/Manager/SceneManager.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

TitleScene::~TitleScene() {

	GameObjectHelper::RemoveObject2D(titleNameId_);
}

void TitleScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
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

	// titleの名前を中心に表示
	titleNameId_ = GameObjectHelper::CreateObject2D("titleName", "titleName");
	Transform2DComponent* titleNameTransform = Component::GetComponent<Transform2DComponent>(titleNameId_);
	titleNameTransform->SetCenterPos();

	// scene遷移用
	fadeTransition_ = std::make_unique<FadeTransition>();
	fadeTransition_->Init();
}

void TitleScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	// スペースキーでGameSceneに遷移する
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		sceneManager->SetNextScene(Scene::Game, std::move(fadeTransition_));
	}

	gameCamera_->Update();
}