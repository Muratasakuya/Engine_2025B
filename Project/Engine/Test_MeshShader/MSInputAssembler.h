#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComPtr.h>

// directX
#include <d3d12.h>

//============================================================================
//	MSInputAssembler class
//============================================================================
class MSInputAssembler {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MSInputAssembler() = default;
	~MSInputAssembler() = default;

	void Create(ID3D12Device* device, class SRVManager* srvManager);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetVertexResource() const { return vertexResource_.Get(); }
	ID3D12Resource* GetIndexResource() const { return indexResource_.Get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12Resource> vertexResource_;
	ComPtr<ID3D12Resource> indexResource_;

	D3D12_GPU_DESCRIPTOR_HANDLE vertexGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE indexGPUHandle_;

	//--------- functions ----------------------------------------------------

	void CreateVertexBuffer(ID3D12Device* device, class SRVManager* srvManager);
	void CreateIndexBuffer(ID3D12Device* device, class SRVManager* srvManager);
};