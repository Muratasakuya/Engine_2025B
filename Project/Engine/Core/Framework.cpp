#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	Framework classMethods
//============================================================================

void Framework::Run() {

	while (true) {

		Update();
		Draw();

		// fullScreen切り替え
		if (Input::GetInstance()->TriggerKey(DIK_F11)) {

			fullscreenEnable_ = !fullscreenEnable_;
			winApp_->SetFullscreen(fullscreenEnable_);
		}

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

	// window作成
	winApp_ = std::make_unique<WinApp>();
	winApp_->Create(width, height, title);

	// scene
	// camera
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Init();
	// light
	lightManager_ = std::make_unique<LightManager>();

	// directXSystem初期化
	InitDirectX(width, height);

	// component機能初期化
	InitComponent();

	// imgui機能初期化
#ifdef _DEBUG
	imguiEditor_ = std::make_unique<ImGuiEditor>();
	imguiEditor_->Init(graphicsCore_->GetRenderTextureGPUHandle(),
		graphicsCore_->GetDebugSceneRenderTextureGPUHandle(),
		graphicsCore_->GetShadowMapGPUHandle());
#endif // _DEBUG

	// scene管理クラス初期化
	sceneManager_ = std::make_unique<SceneManager>(
		Scene::Game, asset_.get(), cameraManager_.get(),
		lightManager_.get(), graphicsCore_->GetPostProcessSystem());

	Input::GetInstance()->Init(winApp_.get());
	LineRenderer::GetInstance()->Init(graphicsCore_->GetDevice(),
		graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Graphics),
		graphicsCore_->GetSRVDescriptor(), graphicsCore_->GetDxShaderCompiler(), cameraManager_.get());

	// 最初からfullScreen設定
	fullscreenEnable_ = true;
	winApp_->SetFullscreen(fullscreenEnable_);

	// assetのeditor初期化
	AssetEditor::GetInstance()->Init(asset_.get());
}

void Framework::InitDirectX(uint32_t width, uint32_t height) {

	// directX初期化
	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(width, height, winApp_.get());

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVDescriptor());
}

void Framework::InitComponent() {

	ECSManager::GetInstance()->Init(graphicsCore_->GetDevice(),
		asset_.get(), graphicsCore_->GetDxCommand());
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	GameTimer::BeginUpdateCount();

	// 描画前処理
	graphicsCore_->BeginFrame();
	// imgui表示更新
#ifdef _DEBUG
	imguiEditor_->Display();
#endif // _DEBUG
	// scene更新
	UpdateScene();

	graphicsCore_->DebugUpdate();
	ECSManager::GetInstance()->Update();

	GameTimer::EndUpdateCount();
}
void Framework::UpdateScene() {

	if (sceneManager_->IsSceneSwitching()) {

		sceneManager_->InitNextScene();
	}
	GameTimer::Update();
	Input::GetInstance()->Update();

	sceneManager_->Update();
	cameraManager_->Update();
	lightManager_->Update();

	CollisionManager::GetInstance()->Update();
}

void Framework::Draw() {

	//========================================================================
	//	draw: endFrame
	//========================================================================

	GameTimer::BeginDrawCount();

	// 描画処理
	graphicsCore_->Render(cameraManager_.get(), lightManager_.get());
	// scene遷移依頼
	sceneManager_->SwitchScene();
	// lineReset
	LineRenderer::GetInstance()->ResetLine();

	GameTimer::EndDrawCount();
}

void Framework::Finalize() {

	graphicsCore_->Finalize(winApp_->GetHwnd());
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();
	ECSManager::GetInstance()->Finalize();

	sceneManager_.reset();
	winApp_.reset();
	asset_.reset();
	graphicsCore_.reset();

	// ComFinalize
	CoUninitialize();
}