#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/BaseDescriptor.h>

//============================================================================
//	DSVDescriptor class
//============================================================================
class DSVDescriptor :
	public BaseDescriptor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DSVDescriptor() :BaseDescriptor(4) {};
	~DSVDescriptor() = default;

	void InitFrameBufferDSV();

	void CreateDSV(uint32_t width, uint32_t height,
		D3D12_CPU_DESCRIPTOR_HANDLE& handle, ComPtr<ID3D12Resource>& resource,
		DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat);

	//--------- accessor -----------------------------------------------------

	void SetFrameGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { dsvGPUHandle_ = handle; }

	ID3D12Resource* GetResource() const { return resource_.Get(); }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetFrameCPUHandle() const { return dsvCPUHandle_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetFrameGPUHandle() const { return dsvGPUHandle_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12Resource> resource_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE dsvGPUHandle_;

	//--------- functions ----------------------------------------------------

	void CreateDepthResource(ComPtr<ID3D12Resource>& resource, uint32_t width, uint32_t height,
		DXGI_FORMAT resourceFormat, DXGI_FORMAT depthClearFormat);
};