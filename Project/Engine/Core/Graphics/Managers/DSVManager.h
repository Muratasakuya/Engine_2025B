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
//	DSVManager class
//============================================================================
class DSVManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DSVManager() = default;
	~DSVManager() = default;

	void Init(uint32_t width, uint32_t height, ID3D12Device* device);

	void CreateDSV(uint32_t width, uint32_t height,
		D3D12_CPU_DESCRIPTOR_HANDLE& handle, ComPtr<ID3D12Resource>& resource,
		DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat);

	//--------- accessor -----------------------------------------------------

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetFrameCPUHandle() const { return dsvCpuHandle_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;

	static const uint32_t kMaxDSVCount_;
	uint32_t useIndex_;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	uint32_t descriptorSize_;

	ComPtr<ID3D12Resource> resource_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle_;

	//--------- functions ----------------------------------------------------

	void CreateDepthResource(ComPtr<ID3D12Resource>& resource, uint32_t width, uint32_t height,
		DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);

	uint32_t Allocate();
};