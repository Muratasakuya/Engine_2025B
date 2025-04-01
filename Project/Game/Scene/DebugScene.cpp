#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessManager.h>
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

	asset->LoadTexture("uvChecker");
	asset->LoadTexture("white");

	asset->LoadModel("teapot");
	asset->LoadModel("plane");

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessManager_ = postProcessManager;

	const std::vector<PostProcess> postProcesses = {

		PostProcess::Bloom,
	};

	postProcessManager_->Create(postProcesses);
	postProcessManager_->AddProcess(PostProcess::Bloom);

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

	object_ = std::make_unique<TemplateObject3D>();

	testEditor_ = std::make_unique<TestEditor>();
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();

	object_->Update();

	LineRenderer::GetInstance()->DrawGrid(16, 16.0f, Color::White());
}