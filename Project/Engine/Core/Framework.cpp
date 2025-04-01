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
		Scene::Debug, asset_.get(), cameraManager_.get());

	Input::GetInstance()->Init(winApp_.get());
	LineRenderer::GetInstance()->Init(graphicsCore_->GetDevice(),
		graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Graphics),
		graphicsCore_->GetDxShaderCompiler(), cameraManager_.get());
}

void Framework::InitDirectX(uint32_t width, uint32_t height) {

	// directX初期化
	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(width, height, winApp_.get(),
		cameraManager_.get());

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVManager());
}

void Framework::InitComponent() {

	// component初期化、CS用のCommandList
	ComponentManager::GetInstance()->Init(
		graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand()->GetCommandList(CommandListType::Compute),
		asset_.get(), graphicsCore_->GetSRVManager(), graphicsCore_->GetRenderObjectManager());

	// 3D
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

	// 2D
	// transform
	transform2DComponentManager_ = std::make_unique<Transform2DManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(transform2DComponentManager_.get());
	// material
	spriteMaterialManager_ = std::make_unique<SpriteMaterialManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(spriteMaterialManager_.get());
	// sprite
	spriteComponentManager_ = std::make_unique<SpriteComponentManager>();
	ComponentManager::GetInstance()->RegisterComponentManager(spriteComponentManager_.get());

	// imgui
	ComponentManager::GetInstance()->InitImGui(
		// 3D
		transform3DComponentManager_.get(), materialManager_.get(),
		modelComponentManager_.get(),
		// 2D
		transform2DComponentManager_.get(), spriteMaterialManager_.get(),
		spriteComponentManager_.get());
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	GameTimer::BeginUpdateCount();

	// 描画前処理
	graphicsCore_->BeginRenderFrame();
	// imgui表示更新
#ifdef _DEBUG
	imguiEditor_->Display();
#endif // _DEBUG
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
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();

	sceneManager_.reset();
	graphicsCore_.reset();
	winApp_.reset();
	asset_.reset();
	transform3DComponentManager_.reset();
	transform2DComponentManager_.reset();
	materialManager_.reset();
	animationComponentManager_.reset();
	modelComponentManager_.reset();
	spriteComponentManager_.reset();

	ComponentManager::GetInstance()->Finalize();

	// ComFinalize
	CoUninitialize();
}