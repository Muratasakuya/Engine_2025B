#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComPtr.h>
#include <Lib/MathUtils/Vector4.h>

// directX
#include <d3d12.h>
// c++
#include <cstdint>

//============================================================================
//	RTVManager class
//============================================================================
class RTVManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RTVManager() = default;
	~RTVManager() = default;

	void Init(ID3D12Device* device);

	void Create(D3D12_CPU_DESCRIPTOR_HANDLE& handle, ID3D12Resource* resource,
		const D3D12_RENDER_TARGET_VIEW_DESC& desc);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;

	static const uint32_t kMaxRTVCount_;
	uint32_t useIndex_;

	ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	uint32_t descriptorSize_;

	//--------- functions ----------------------------------------------------

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);

	uint32_t Allocate();
};