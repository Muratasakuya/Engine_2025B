#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Component/EntityComponent.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	Framework classMethods
//============================================================================

void Framework::Run() {

	while (!winApp_->ProcessMessage()) {

		Update();
		Draw();
	}

	Finalize();
}

Framework::Framework(uint32_t width, uint32_t height, const wchar_t* title) {

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//========================================================================
	//	engineConfig
	//========================================================================

	Logger::EnableFileLogging();
	Logger::Log("[StartLogginig]");

	//========================================================================
	//	init
	//========================================================================

	winApp_ = std::make_unique<WinApp>();
	winApp_->Create(width, height, title);

	// setRenderer
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Init();

	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(width, height, winApp_.get(),
		cameraManager_.get());

	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVManager());

	EntityComponent::GetInstance()->Init(graphicsCore_->GetDevice(),
		asset_.get(), graphicsCore_->GetSRVManager());

	sceneManager_ = std::make_unique<SceneManager>(
		Scene::Debug, asset_.get(), cameraManager_.get());
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	// scene更新
	UpdateScene();
	// entityBuffer更新
	EntityComponent::GetInstance()->Update();
}
void Framework::UpdateScene() {

	if (sceneManager_->IsSceneSwitching()) {

		sceneManager_->InitNextScene();
	}
	GameTimer::Update();
	sceneManager_->Update();
	cameraManager_->Update();
}

void Framework::Draw() {

	//========================================================================
	//	draw: endFrame
	//========================================================================

	// 描画処理
	graphicsCore_->Render();
	// scene遷移依頼
	sceneManager_->SwitchScene();
}

void Framework::Finalize() {

	graphicsCore_->Finalize(winApp_->GetHwnd());
	EntityComponent::GetInstance()->Finalize();

	graphicsCore_.reset();
	winApp_.reset();
	asset_.reset();

	// ComFinalize
	CoUninitialize();
}