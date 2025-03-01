#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComPtr.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>

//============================================================================
//	DxUtils namespace
//============================================================================
namespace DxUtils {

	//--------- functions ----------------------------------------------------

	void MakeDescriptorHeap(ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		ID3D12Device* device, const D3D12_DESCRIPTOR_HEAP_DESC& desc);

	void CreateBufferResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, size_t sizeInBytes);

	bool CanAllocateIndex(uint32_t useIndex, uint32_t kMaxCount);
};