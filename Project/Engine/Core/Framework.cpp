#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Component/Manager/ComponentManager.h>
#include <Engine/Component/Manager/CollisionManager.h>
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

	// window作成
	winApp_ = std::make_unique<WinApp>();
	winApp_->Create(width, height, title);

	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Init();

	// directX初期化
	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(width, height, winApp_.get(),
		cameraManager_.get());

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVManager());

	// component機能初期化
	InitComponent();

	// imgui機能初期化
	imguiEditor_ = std::make_unique<ImGuiEditor>();
	imguiEditor_->Init(graphicsCore_->GetRenderTextureGPUHandle(),
		graphicsCore_->GetDebugSceneRenderTextureGPUHandle(),
		graphicsCore_->GetShadowMapGPUHandle());

	// scene管理クラス初期化
	sceneManager_ = std::make_unique<SceneManager>(
		Scene::Debug, asset_.get(), cameraManager_.get());

	Input::GetInstance()->Init(winApp_.get());
	LineRenderer::GetInstance()->Init(graphicsCore_->GetDevice(),
		graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Graphics),
		graphicsCore_->GetDxShaderCompiler(), cameraManager_.get());
}

void Framework::InitComponent() {

	// component初期化、CS用のCommandList
	ComponentManager::GetInstance()->Init(
		graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Compute),
		asset_.get(), graphicsCore_->GetSRVManager(), graphicsCore_->GetRenderObjectManager());

	// transform
	transform3DComponentManager_ = std::make_unique<Transform3DManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(transform3DComponentManager_.get());
	// material
	materialManager_ = std::make_unique<MaterialManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(materialManager_.get());
	// animation
	animationComponentManager_ = std::make_unique<AnimationComponentManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(animationComponentManager_.get());
	// model
	modelComponentManager_ = std::make_unique<ModelComponentManager>();
	modelComponentManager_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxShaderCompiler());
	ComponentManager::GetInstance()->RegisterComponentManager(modelComponentManager_.get());

	// imgui
	ComponentManager::GetInstance()->InitImGui(transform3DComponentManager_.get(),
		materialManager_.get(), modelComponentManager_.get());
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	GameTimer::BeginUpdateCount();

	// 描画前処理
	graphicsCore_->BeginRenderFrame();
	// imgui表示更新
	imguiEditor_->Display();
	// scene更新
	UpdateScene();
	// entityBuffer更新
	ComponentManager::GetInstance()->Update();

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

	CollisionManager::GetInstance()->Update();
}

void Framework::Draw() {

	//========================================================================
	//	draw: endFrame
	//========================================================================

	GameTimer::BeginDrawCount();

	// 描画処理
	graphicsCore_->Render();
	// scene遷移依頼
	sceneManager_->SwitchScene();
	// lineReset
	LineRenderer::GetInstance()->ResetLine();

	GameTimer::EndDrawCount();
}

void Framework::Finalize() {

	graphicsCore_->Finalize(winApp_->GetHwnd());
	ComponentManager::GetInstance()->Finalize();
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();

	graphicsCore_.reset();
	winApp_.reset();
	asset_.reset();
	transform3DComponentManager_.reset();
	materialManager_.reset();
	animationComponentManager_.reset();
	modelComponentManager_.reset();

	// ComFinalize
	CoUninitialize();
}