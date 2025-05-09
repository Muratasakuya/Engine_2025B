#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Core/Component/ECS/ComponentManager.h>
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

	// editor初期化
	primitiveEditor_ = std::make_unique<PrimitiveEditor>();
	primitiveEditor_->Init(asset_.get());

	// 最初からfullScreen設定
	fullscreenEnable_ = true;
	winApp_->SetFullscreen(fullscreenEnable_);
}

void Framework::InitDirectX(uint32_t width, uint32_t height) {

	// directX初期化
	graphicsCore_ = std::make_unique<GraphicsCore>();
	graphicsCore_->Init(width, height, winApp_.get());

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(graphicsCore_->GetDevice(), graphicsCore_->GetDxCommand(),
		graphicsCore_->GetSRVDescriptor());

	// renderer初期化
	graphicsCore_->InitRenderer(asset_.get());
}

void Framework::InitComponent() {

	// component初期化、CS用のCommandList
	ComponentManager::GetInstance()->Init(
		graphicsCore_->GetDevice(), asset_.get(), graphicsCore_->GetGPUObjectSystem());

	// 3D
	// transform
	transform3DStore_ = std::make_unique<Transform3DStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(transform3DStore_.get());
	// material
	materialStore_ = std::make_unique<MaterialStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(materialStore_.get());
	// animation
	animationStore_ = std::make_unique<AnimationStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(animationStore_.get());

	// effect
	// transform
	effectTransformStore_ = std::make_unique<EffectTransformStore>();
	effectTransformStore_->Init(cameraManager_.get());
	ComponentManager::GetInstance()->RegisterComponentStore(effectTransformStore_.get());
	// material
	effectMaterialStore_ = std::make_unique<EffectMaterialStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(effectMaterialStore_.get());
	// transform
	primitiveMeshStore_ = std::make_unique<PrimitiveMeshStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(primitiveMeshStore_.get());

	// 2D
	// transform
	transform2DStore_ = std::make_unique<Transform2DStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(transform2DStore_.get());
	// material
	spriteMaterialStore_ = std::make_unique<SpriteMaterialStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(spriteMaterialStore_.get());
	// sprite
	spriteStore_ = std::make_unique<SpriteStore>();
	ComponentManager::GetInstance()->RegisterComponentStore(spriteStore_.get());
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

	sceneManager_.reset();
	primitiveMeshStore_.reset();
	spriteStore_.reset();
	graphicsCore_.reset();
	winApp_.reset();
	asset_.reset();

	ComponentManager::GetInstance()->Finalize();

	// ComFinalize
	CoUninitialize();
}