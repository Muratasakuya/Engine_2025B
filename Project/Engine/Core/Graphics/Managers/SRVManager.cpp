#include "SRVManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/DxUtils.h>
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	SRVManager classMethods
//============================================================================

const uint32_t SRVManager::kMaxSRVCount_ = 128;

void SRVManager::Init(ID3D12Device* device) {

	device_ = nullptr;
	device_ = device;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};

	// SRV
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = kMaxSRVCount_;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// descriptor生成
	DxUtils::MakeDescriptorHeap(descriptorHeap_, device, heapDesc);
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
}

void SRVManager::CreateSRV(uint32_t& srvIndex, ID3D12Resource* resource,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {

	srvIndex = Allocate(); // 参照で飛び出し側のIndexも設定する
	device_->CreateShaderResourceView(resource, &desc, GetCPUHandle(srvIndex));
}

void SRVManager::CreateUAV(uint32_t& uavIndex, ID3D12Resource* resource,
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc) {

	uavIndex = Allocate(); // 参照で飛び出し側のIndexも設定する
	device_->CreateUnorderedAccessView(resource, nullptr, &desc, GetCPUHandle(uavIndex));
}

void SRVManager::IncrementIndex() {

	if (!DxUtils::CanAllocateIndex(useIndex_, kMaxSRVCount_)) {
		ASSERT(FALSE, "Cannot allocate more SRVs, maximum count reached");
	}
	useIndex_++;
}

uint32_t SRVManager::Allocate() {

	if (!DxUtils::CanAllocateIndex(useIndex_, kMaxSRVCount_)) {
		ASSERT(FALSE, "Cannot allocate more SRVs, maximum count reached");
	}

	int index = useIndex_;
	useIndex_++;

	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUHandle(uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_.Get()->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle(uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_.Get()->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}
