#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Component/Manager/ComponentManager.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	Framework classMethods
//============================================================================

void Framework::Run() {

	while (true) {

		Update();
		Draw();

		if (winApp_->ProcessMessage() ||
			Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			break;
		}
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

	imguiEditor_ = std::make_unique<ImGuiEditor>();
	imguiEditor_->Init(graphicsCore_->GetRenderTextureGPUHandle(),
		graphicsCore_->GetDebugSceneRenderTextureGPUHandle(),
		graphicsCore_->GetShadowMapGPUHandle());

	ComponentManager::GetInstance()->Init(graphicsCore_->GetDevice(),
		asset_.get(), graphicsCore_->GetSRVManager());

	sceneManager_ = std::make_unique<SceneManager>(
		Scene::Debug, asset_.get(), cameraManager_.get());

	Input::GetInstance()->Init(winApp_.get());

	LineRenderer::GetInstance()->Init(graphicsCore_->GetDevice(),
		graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Graphics),
		graphicsCore_->GetDxShaderCompiler(), cameraManager_.get());
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	// 描画前処理
	graphicsCore_->BeginRenderFrame();
	// imgui表示更新
	imguiEditor_->Display();
	// scene更新
	UpdateScene();
	// entityBuffer更新
	ComponentManager::GetInstance()->Update();
}
void Framework::UpdateScene() {

	if (sceneManager_->IsSceneSwitching()) {

		sceneManager_->InitNextScene();
	}
	GameTimer::Update();
	Input::GetInstance()->Update();

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
	// lineReset
	LineRenderer::GetInstance()->ResetLine();
}

void Framework::Finalize() {

	graphicsCore_->Finalize(winApp_->GetHwnd());
	ComponentManager::GetInstance()->Finalize();
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();

	graphicsCore_.reset();
	winApp_.reset();
	asset_.reset();

	// ComFinalize
	CoUninitialize();
}