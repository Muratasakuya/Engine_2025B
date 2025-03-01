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
//	SRVManager class
//============================================================================
class SRVManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SRVManager() = default;
	~SRVManager() = default;

	void Init(ID3D12Device* device);

	void CreateSRV(uint32_t& srvIndex, ID3D12Resource* resource,
		const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

	void CreateUAV(uint32_t& uavIndex, ID3D12Resource* resource,
		const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc);

	void IncrementIndex();

	//--------- accessor -----------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);

	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;

	static const uint32_t kMaxSRVCount_;
	uint32_t useIndex_;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	uint32_t descriptorSize_;

	//--------- functions ----------------------------------------------------

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);

	uint32_t Allocate();
};