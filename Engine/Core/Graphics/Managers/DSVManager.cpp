#include "DSVManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxUtils.h>
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	DSVManager classMethods
//============================================================================

const uint32_t DSVManager::kMaxDSVCount_ = 2;

void DSVManager::CreateDepthResource(ComPtr<ID3D12Resource>& resource,
	uint32_t width, uint32_t height,
	DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat) {

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                                   // Textureの幅
	resourceDesc.Height = height;                                 // Textureの高さ
	resourceDesc.MipLevels = 1;                                   // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                            // 奥行　or 配列Textureの配列数
	resourceDesc.Format = resourceFormat;                         // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;                            // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = depthClearFormat; // フォーマット、Resourceに合わせる

	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,                  // Heapの設定
		D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定、特になし
		&resourceDesc,                    // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue,                 // Clear最適値
		IID_PPV_ARGS(&resource)           // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
}

void DSVManager::Init(uint32_t width, uint32_t height, ID3D12Device* device) {

	device_ = nullptr;
	device_ = device;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};

	// DSV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	heapDesc.NumDescriptors = kMaxDSVCount_;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// descriptor生成
	DxUtils::MakeDescriptorHeap(descriptorHeap_, device, heapDesc);
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(heapDesc.Type);

	// frameBuffer
	CreateDSV(width, height, dsvCpuHandle_, resource_,
		DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void DSVManager::CreateDSV(uint32_t width, uint32_t height,
	D3D12_CPU_DESCRIPTOR_HANDLE& handle, ComPtr<ID3D12Resource>& resource,
	DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat) {

	uint32_t index = Allocate();
	// 参照で飛び出し側も設定する
	handle = GetCPUHandle(index);

	CreateDepthResource(resource, width, height,
		resourceFormat, depthClearFormat);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = depthClearFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device_->CreateDepthStencilView(resource.Get(), &dsvDesc, handle);
}

uint32_t DSVManager::Allocate() {

	if (!DxUtils::CanAllocateIndex(useIndex_, kMaxDSVCount_)) {
		ASSERT(FALSE, "Cannot allocate more DSVs, maximum count reached");
	}

	int index = useIndex_;
	useIndex_++;

	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE DSVManager::GetCPUHandle(uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DSVManager::GetGPUHandle(uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}