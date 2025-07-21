#include "Framework.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Input/Input.h>
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Effect/Particle/Core/ParticleManager.h>
#include <Engine/Config.h>
#include <Engine/Utility/GameTimer.h>

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

	// srvDescriptorHeap設定、最初のみ設定
	dxCommand->SetDescriptorHeaps({ srvDescriptor->GetDescriptorHeap() });

	//------------------------------------------------------------------------
	// particle機能初期化

	ParticleManager::GetInstance()->Init(asset_.get(), device,
		srvDescriptor, shaderCompiler, sceneView_.get());
	ParticleManager::GetInstance()->InitParticle(dxCommand);

	//------------------------------------------------------------------------
	// object機能初期化

	ObjectManager::GetInstance()->Init(device, asset_.get(), dxCommand);

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

	// data更新
	ObjectManager::GetInstance()->UpdateData();
	// particle更新
	ParticleManager::GetInstance()->Update(graphicsPlatform_->GetDxCommand());
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
#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	renderEngine_->Rendering(RenderEngine::ViewType::Debug);

	// bloom処理のみを行う
	postProcessSystem_->ExecuteDebugScene(renderEngine_->GetRenderTexture(
		RenderEngine::ViewType::Debug)->GetSRVGPUHandle(), dxCommand);
#endif
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
}

void Framework::Finalize() {

	graphicsPlatform_->Finalize(winApp_->GetHwnd());
	renderEngine_->Finalize();
	Input::GetInstance()->Finalize();
	LineRenderer::GetInstance()->Finalize();

	sceneManager_.reset();

	ParticleManager::GetInstance()->Finalize();
	ObjectManager::GetInstance()->Finalize();

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