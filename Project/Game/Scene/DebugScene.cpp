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

	asset->LoadModel("walk");
	asset->LoadAnimation("walk", "walk");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	// iの数だけ生成する
	// 128 ...25fpsくらい
	// debugCameraと合わせて計 *2回描画
	for (uint32_t i = 0; i < 128; ++i) {

		ComponentManager::GetInstance()->CreateObject3D("walk", std::nullopt, "walk");
	}
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();

	LineRenderer::GetInstance()->DrawGrid(16, 16.0f, Color::White());
}