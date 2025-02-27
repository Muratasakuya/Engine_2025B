#include "GraphicsCore.h"

//============================================================================
//	include
//============================================================================

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

//============================================================================
//	GraphicsCore classMethods
//============================================================================

void GraphicsCore::InitDXDevice() {
#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();

		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	dxDevice_->Create();

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(dxDevice_->Get()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {

			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

void GraphicsCore::Init(uint32_t width, uint32_t height,
	WinApp* winApp, CameraManager* cameraManager) {

	windowWidth_ = width;
	windowHeight_ = height;

	shadowMapWidth_ = 2048;
	shadowMapHeight_ = 2048;

	windowClearColor_ = Color(0.04f, 0.04f, 1.0f, 1.0f);

	//============================================================================
	//	init: directX
	//============================================================================

	// device初期化
	dxDevice_ = std::make_unique<DxDevice>();
	InitDXDevice();
	ID3D12Device* device = dxDevice_->Get();

	// command初期化
	dxCommand_ = std::make_unique<DxCommand>();
	dxCommand_->Create(device);

	// RTV初期化
	rtvManager_ = std::make_unique<RTVManager>();
	rtvManager_->Init(device);

	// DXC初期化
	dxShaderComplier_ = std::make_unique<DxShaderCompiler>();
	dxShaderComplier_->Init();

	// 画面設定
	dxSwapChain_ = std::make_unique<DxSwapChain>();
	dxSwapChain_->Create(width, height, windowClearColor_, winApp,
		dxDevice_->GetDxgiFactory(), dxCommand_->GetQueue(), rtvManager_.get());

	// DSV初期化
	dsvManager_ = std::make_unique<DSVManager>();
	dsvManager_->Init(width, height, dxDevice_->Get());

	// SRV初期化
	srvManager_ = std::make_unique<SRVManager>();
	srvManager_->Init(dxDevice_->Get());

	// renderTexture作成
	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Create(width, height, windowClearColor_, DXGI_FORMAT_R32G32B32A32_FLOAT,
		device, rtvManager_.get(), srvManager_.get());

	// shadowMap作成
	shadowMap_ = std::make_unique<ShadowMap>();
	shadowMap_->Create(shadowMapWidth_, shadowMapHeight_,
		dsvManager_.get(), srvManager_.get());

	// mesh描画初期化
	meshRenderer_ = std::make_unique<MeshRenderer>();
	meshRenderer_->Init(dxCommand_.get(), device,
		shadowMap_.get(), dxShaderComplier_.get(),
		cameraManager);

	// offscreen初期化
	offscreenPipeline_ = std::make_unique<PipelineState>();
	offscreenPipeline_->Create("CopyTexture.json", device, dxShaderComplier_.get());
}

void GraphicsCore::Finalize(HWND hwnd) {

	dxCommand_->Finalize(hwnd);

	dxDevice_.reset();
	dxCommand_.reset();
	rtvManager_.reset();
	dxSwapChain_.reset();
	dsvManager_.reset();
	srvManager_.reset();
	dxShaderComplier_.reset();
	renderTexture_.reset();
	shadowMap_.reset();
	meshRenderer_.reset();
}

//============================================================================
//	Rendering Pass
//============================================================================

void GraphicsCore::Render() {

	// 描画前処理
	BeginRenderFrame();

	// zPass
	RenderZPass();
	// offscreenTexture
	RenderOffscreenTexture();
	// frameBuffer
	RenderFrameBuffer();

	// commandの実行
	EndRenderFrame();
}

//============================================================================
//	Begin
//============================================================================

void GraphicsCore::BeginRenderFrame() {

	// srvDescriptorHeap設定
	dxCommand_->SetDescriptorHeaps({ srvManager_->GetDescriptorHeap() });
	// bufferの更新
	meshRenderer_->Update();
}

//============================================================================
//	Main
//============================================================================

void GraphicsCore::RenderZPass() {

	dxCommand_->SetRenderTargets(std::nullopt, shadowMap_->GetCPUHandle());
	dxCommand_->ClearDepthStencilView(shadowMap_->GetCPUHandle());
	dxCommand_->SetViewportAndScissor(shadowMapWidth_, shadowMapHeight_);

	// Z値描画
	meshRenderer_->RenderZPass();

	// Write -> PixelShader
	dxCommand_->TransitionBarriers({ shadowMap_->GetResource() },
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GraphicsCore::RenderOffscreenTexture() {

	dxCommand_->SetRenderTargets(renderTexture_->GetRenderTarget(),
		dsvManager_->GetFrameCPUHandle());
	dxCommand_->ClearDepthStencilView(dsvManager_->GetFrameCPUHandle());
	dxCommand_->SetViewportAndScissor(windowWidth_, windowHeight_);

	// 通常描画処理
	meshRenderer_->Render();

	// RenderTarget -> PixelShader
	dxCommand_->TransitionBarriers({ renderTexture_->GetResource() },
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GraphicsCore::RenderFrameBuffer() {

	// RenderTarget -> Present
	dxCommand_->TransitionBarriers({ dxSwapChain_->GetCurrentResource() },
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	dxCommand_->SetRenderTargets(dxSwapChain_->GetRenderTarget(),
		dsvManager_->GetFrameCPUHandle());
	dxCommand_->SetViewportAndScissor(windowWidth_, windowHeight_);

	const UINT vertexCount = 3;

	auto commandList = dxCommand_->GetCommandList(CommandListType::Graphics);
	commandList->SetGraphicsRootSignature(offscreenPipeline_->GetRootSignature());
	commandList->SetPipelineState(offscreenPipeline_->GetGraphicsPipeline());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootDescriptorTable(0, renderTexture_->GetGPUHandle());
	commandList->DrawInstanced(vertexCount, 1, 0, 0);
}

//============================================================================
//	End
//============================================================================

void GraphicsCore::EndRenderFrame() {

	// ComputeCommand実行
	dxCommand_->ExecuteComputeCommands();
	// PixelShader -> Write
	dxCommand_->TransitionBarriers({ shadowMap_->GetResource() },
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	// PixelShader -> RenderTarget
	dxCommand_->TransitionBarriers({ renderTexture_->GetResource() },
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// Present -> RenderTarget
	dxCommand_->TransitionBarriers({ dxSwapChain_->GetCurrentResource() },
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	// GraphicsCommand実行
	dxCommand_->ExecuteGraphicsCommands(dxSwapChain_->Get());
}