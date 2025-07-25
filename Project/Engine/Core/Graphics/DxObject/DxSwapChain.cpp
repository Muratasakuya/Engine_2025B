#include "DxSwapChain.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxObject/DxDevice.h>
#include <Engine/Core/Graphics/Descriptors/RTVDescriptor.h>
#include <Engine/Core/Window/WinApp.h>
#include <Engine/Config.h>

//============================================================================
//	DxSwapChain classMethods
//============================================================================

void DxSwapChain::Create(WinApp* winApp, IDXGIFactory7* factory,
	ID3D12CommandQueue* queue, RTVDescriptor* rtvDescriptor) {

	// renderTargetの設定
	renderTarget_.width = Config::kWindowWidth;
	renderTarget_.height = Config::kWindowHeight;
	renderTarget_.clearColor = Color(
		Config::kWindowClearColor[0],
		Config::kWindowClearColor[1],
		Config::kWindowClearColor[2],
		Config::kWindowClearColor[3]);

	swapChain_ = nullptr;
	desc_.Width = Config::kWindowWidth;
	desc_.Height = Config::kWindowHeight;
	desc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc_.SampleDesc.Count = 1;
	desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc_.BufferCount = kBufferCount;
	desc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//desc_.Scaling = DXGI_SCALING_NONE;
	//desc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	HRESULT hr = factory->CreateSwapChainForHwnd(
		queue, winApp->GetHwnd(), &desc_, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// RTVDescの設定
	rtvDesc.Format = Config::kSwapChainRTVFormat;          // 出力結果をSRGBも変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む

	for (uint32_t index = 0; index < kBufferCount; ++index) {

		hr = swapChain_->GetBuffer(index, IID_PPV_ARGS(&resources_[index]));
		assert(SUCCEEDED(hr));
		resources_[index]->SetName((L"backBufferResource" + std::to_wstring(index)).c_str());

		// RTV作成
		rtvDescriptor->Create(rtvHandles_[index], resources_[index].Get(), rtvDesc);
	}
}

ID3D12Resource* DxSwapChain::GetCurrentResource() const {

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	return resources_[backBufferIndex].Get();
}

const RenderTarget& DxSwapChain::GetRenderTarget() {

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	renderTarget_.rtvHandle.ptr = rtvHandles_[backBufferIndex].ptr;

	return renderTarget_;
}