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

	asset->LoadModel("teapot");
	asset->LoadModel("suzanne");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	object_ = std::make_unique<TemplateObject3D>();
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();

	object_->Update();

	LineRenderer::GetInstance()->DrawGrid(16, 16.0f, Color::White());
}