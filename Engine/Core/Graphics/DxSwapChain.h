#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxStructures.h>
#include <Engine/Core/Lib/ComPtr.h>

// driectX
#include <d3d12.h>
#include <dxgi1_6.h>
// c++
#include <array>
// front
class RTVManager;
class DxDevice;
class DxCommand;

//============================================================================
//	DxSwapChain class
//============================================================================
class DxSwapChain {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DxSwapChain() = default;
	~DxSwapChain() = default;

	void Create(uint32_t width, uint32_t height, Color clearColor, class WinApp* winApp,
		IDXGIFactory7* factory, ID3D12CommandQueue* queue, RTVManager* rtvManager);

	//--------- accessor -----------------------------------------------------

	IDXGISwapChain4* Get() const { return swapChain_.Get(); }

	ID3D12Resource* GetCurrentResource() const;

	const RenderTarget& GetRenderTarget();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	RenderTarget renderTarget_;

	// doubleBuffer
	static const constexpr uint32_t kBufferCount = 2;

	ComPtr<IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 desc_{};

	std::array<ComPtr<ID3D12Resource>, kBufferCount> resources_;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kBufferCount> rtvHandles_;
};