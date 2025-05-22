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
#include <Engine/Particle/ParticleSystem.h>
#include <Engine/Config.h>
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

Framework::Framework() {

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//========================================================================
	//	engineConfig
	//========================================================================

	Logger::EnableFileLogging();
	Logger::Log("[StartLogginig]");

	//========================================================================
	//	init
	//========================================================================

	fullscreenEnable_ = Config::kFullscreenEnable;

	// window作成
	winApp_ = std::make_unique<WinApp>();
	winApp_->Create();

	//------------------------------------------------------------------------
	// scene初期化

	// camera
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Init();
	// light
	lightManager_ = std::make_unique<LightManager>();

	//------------------------------------------------------------------------
	// directX初期化

	// directX初期化
	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(winApp_.get());

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVDescriptor());

	// fullScreen設定
	winApp_->SetFullscreen(fullscreenEnable_);

	//------------------------------------------------------------------------
	// particle機能初期化

	ParticleSystem::GetInstance()->Init(asset_.get(),
		graphicsCore_->GetDevice(), cameraManager_.get());

	//------------------------------------------------------------------------
	// component機能初期化

	ECSManager::GetInstance()->Init(graphicsCore_->GetDevice(),
		asset_.get(), graphicsCore_->GetDxCommand());

	//------------------------------------------------------------------------
	// scene管理クラス初期化

	sceneManager_ = std::make_unique<SceneManager>(
		Scene::Game, asset_.get(), cameraManager_.get(),
		lightManager_.get(), graphicsCore_->GetPostProcessSystem());

	//------------------------------------------------------------------------
	// module初期化

	Input::GetInstance()->Init(winApp_.get());
	LineRenderer::GetInstance()->Init(graphicsCore_->GetDevice(),
		graphicsCore_->GetDxCommand()->GetCommandList(),
		graphicsCore_->GetSRVDescriptor(), graphicsCore_->GetDxShaderCompiler(), cameraManager_.get());
	AssetEditor::GetInstance()->Init(asset_.get());

	//------------------------------------------------------------------------
	// imgui機能初期化

#ifdef _DEBUG
	imguiEditor_ = std::make_unique<ImGuiEditor>();
	imguiEditor_->Init(graphicsCore_->GetRenderTextureGPUHandle(),
		graphicsCore_->GetDebugSceneRenderTextureGPUHandle(),
		graphicsCore_->GetShadowMapGPUHandle());
#endif // _DEBUG
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

	GameTimer::EndUpdateCount();
}
void Framework::UpdateScene() {

	if (sceneManager_->IsSceneSwitching()) {

		sceneManager_->InitNextScene();
	}
	GameTimer::Update();
	Input::GetInstance()->Update();

	// scene更新
	sceneManager_->Update();
	cameraManager_->Update();
	lightManager_->Update();

	// component更新
	ECSManager::GetInstance()->UpdateComponent();
	// particle更新
	ParticleSystem::GetInstance()->Update();
	// collision更新
	CollisionManager::GetInstance()->Update();
}

void Framework::Draw() {

	//========================================================================
	//	draw: endFrame
	//========================================================================

	GameTimer::BeginDrawCount();

	graphicsCore_->DebugUpdate();
	// GPUBuffer転送
	ECSManager::GetInstance()->UpdateBuffer();

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
	ParticleSystem::GetInstance()->Finalize();
	ECSManager::GetInstance()->Finalize();

	sceneManager_.reset();
	winApp_.reset();
	asset_.reset();
	graphicsCore_.reset();

	// ComFinalize
	CoUninitialize();
}