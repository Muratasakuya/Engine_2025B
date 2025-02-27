#include "RTVManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxUtils.h>
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	RTVManager classMethods
//============================================================================

const uint32_t RTVManager::kMaxRTVCount_ = 12;

void RTVManager::Init(ID3D12Device* device) {

	device_ = nullptr;
	device_ = device;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};

	// RTV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = kMaxRTVCount_;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// descriptor生成
	DxUtils::MakeDescriptorHeap(descriptorHeap_, device, heapDesc);
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
}

void RTVManager::Create(D3D12_CPU_DESCRIPTOR_HANDLE& handle, ID3D12Resource* resource,
	const D3D12_RENDER_TARGET_VIEW_DESC& desc) {

	uint32_t index = Allocate();
	// 参照で飛び出し側のhandleも設定する
	handle = GetCPUHandle(index);
	device_->CreateRenderTargetView(resource, &desc, handle);
}

uint32_t RTVManager::Allocate() {

	if (!DxUtils::CanAllocateIndex(useIndex_, kMaxRTVCount_)) {
		ASSERT(FALSE, "Cannot allocate more RTVs, maximum count reached");
	}

	int index = useIndex_;
	useIndex_++;

	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetCPUHandle(uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}