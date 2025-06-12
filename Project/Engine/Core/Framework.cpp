#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Particle/ParticleSystem.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Config.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	Framework classMethods
//============================================================================

void Framework::Run() {

	while (true) {

		Update();

		Draw();

		EndRequest();

		// fullScreen切り替え
		if (Input::GetInstance()->TriggerKey(DIK_F11)) {

			fullscreenEnable_ = !fullscreenEnable_;
			winApp_->SetFullscreen(fullscreenEnable_);
		}

		if (winApp_->ProcessMessage()) {
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

	sceneView_ = std::make_unique<SceneView>();
	sceneView_->Init();

	//------------------------------------------------------------------------
	// directX機能初期化

	graphicsPlatform_ = std::make_unique<GraphicsPlatform>();
	graphicsPlatform_->Init();

	ID3D12Device8* device = graphicsPlatform_->GetDevice();
	DxCommand* dxCommand = graphicsPlatform_->GetDxCommand();
	DxShaderCompiler* shaderCompiler = graphicsPlatform_->GetDxShaderCompiler();

	renderEngine_ = std::make_unique<RenderEngine>();
	renderEngine_->Init(winApp_.get(), device, shaderCompiler, dxCommand, graphicsPlatform_->GetDxgiFactory());

	SRVDescriptor* srvDescriptor = renderEngine_->GetSRVDescriptor();

	postProcessSystem_ = std::make_unique<PostProcessSystem>();
	postProcessSystem_->Init(device, shaderCompiler, srvDescriptor);

	// asset機能初期化
	asset_ = std::make_unique<Asset>();
	asset_->Init(device, dxCommand, srvDescriptor);

	// fullScreen設定
	winApp_->SetFullscreen(fullscreenEnable_);

	//------------------------------------------------------------------------
	// particle機能初期化

	ParticleSystem::GetInstance()->Init(asset_.get(), device,
		srvDescriptor, shaderCompiler, sceneView_.get());

	//------------------------------------------------------------------------
	// component機能初期化

	ECSManager::GetInstance()->Init(device, asset_.get(), dxCommand);

	//------------------------------------------------------------------------
	// scene管理クラス初期化

	sceneManager_ = std::make_unique<SceneManager>(Scene::Game,
		asset_.get(), postProcessSystem_.get(), sceneView_.get());

	//------------------------------------------------------------------------
	// module初期化

	Input::GetInstance()->Init(winApp_.get());
	LineRenderer::GetInstance()->Init(device, dxCommand->GetCommandList(),
		srvDescriptor, shaderCompiler, sceneView_.get());
	AssetEditor::GetInstance()->Init(asset_.get());

	//------------------------------------------------------------------------
	// imgui機能初期化

#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	imguiEditor_ = std::make_unique<ImGuiEditor>();
	imguiEditor_->Init(renderEngine_->GetRenderTextureGPUHandle(), postProcessSystem_->GetDebugSceneGPUHandle());
#endif
}

void Framework::Update() {

	//========================================================================
	//	update
	//========================================================================

	GameTimer::BeginFrameCount();
	GameTimer::BeginUpdateCount();

	// 描画前処理
	renderEngine_->BeginFrame();
	// imgui表示更新
#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	imguiEditor_->Display();
#endif
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
	sceneView_->Update();

	// component更新
	ECSManager::GetInstance()->UpdateComponent();
	// particle更新
	ParticleSystem::GetInstance()->Update();
	// collision更新
	CollisionManager::GetInstance()->Update();
}

void Framework::Draw() {

	DxCommand* dxCommand = graphicsPlatform_->GetDxCommand();

	//========================================================================
	//	draw: endFrame
	//========================================================================

	GameTimer::BeginDrawCount();

	// GPUの更新処理
	renderEngine_->UpdateGPUBuffer(sceneView_.get());

	//========================================================================
	//	draw: render
	//========================================================================

	// 描画処理
	RenderPath(dxCommand);

	//========================================================================
	//	draw: execute
	//========================================================================

	renderEngine_->EndRenderFrameBuffer();

	// csへの書き込み状態へ遷移
	postProcessSystem_->ToWrite(dxCommand);

	// command実行
	dxCommand->ExecuteCommands(renderEngine_->GetDxSwapChain()->Get());

	GameTimer::EndDrawCount();
	GameTimer::EndFrameCount();
}

void Framework::RenderPath(DxCommand* dxCommand) {

	//========================================================================
	//	draw: renderTexture
	//========================================================================

	renderEngine_->Rendering(RenderEngine::ViewType::Main);

	// postProcess処理実行
	postProcessSystem_->Execute(renderEngine_->GetRenderTexture(
		RenderEngine::ViewType::Main)->GetSRVGPUHandle(), dxCommand);

	//========================================================================
	//	draw: debugViewRenderTexture
	//========================================================================

	renderEngine_->Rendering(RenderEngine::ViewType::Debug);

	// bloom処理を行う
	postProcessSystem_->ExecuteDebugScene(renderEngine_->GetRenderTexture(
		RenderEngine::ViewType::Debug)->GetSRVGPUHandle(), dxCommand);

	//========================================================================
	//	draw: frameBuffer
	//========================================================================

	renderEngine_->BeginRenderFrameBuffer();

	// frameBufferへ結果を描画
	postProcessSystem_->RenderFrameBuffer(dxCommand);
}

void Framework::EndRequest() {

	// scene遷移依頼
	sceneManager_->SwitchScene();
	// lineReset
	LineRenderer::GetInstance()->ResetLine();
	// particleReset
	ParticleSystem::GetInstance()->ResetParticleData();
}

void Framework::Finalize() {

	graphicsPlatform_->Finalize(winApp_->GetHwnd());
	renderEngine_->Finalize();
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();
	Skybox::GetInstance()->Finalize();

	sceneManager_.reset();

	ParticleSystem::GetInstance()->Finalize();
	ECSManager::GetInstance()->Finalize();

	winApp_.reset();
	asset_.reset();
	graphicsPlatform_.reset();
	renderEngine_.reset();
	postProcessSystem_.reset();
	sceneView_.reset();
	imguiEditor_.reset();

	// ComFinalize
	CoUninitialize();
}