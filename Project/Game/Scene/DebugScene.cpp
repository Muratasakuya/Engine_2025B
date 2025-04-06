#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessManager.h>
#include <Engine/Core/Component/User/ComponentHelper.h>
#include <Game/Camera/Manager/CameraManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	DebugScene classMethods
//============================================================================

void DebugScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] PostProcessManager* postProcessManager
) {

	//========================================================================
	//	load
	//========================================================================

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessManager_ = postProcessManager;

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();
}