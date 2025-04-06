#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>

//============================================================================
//	IOVertexBuffer class
//============================================================================
class IOVertexBuffer :
	public DxConstBuffer<ModelVertexData> {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IOVertexBuffer() = default;
	~IOVertexBuffer() = default;

	void Init(UINT vertexNum, ID3D12Resource* vertexResource,
		ID3D12Device* device, class SRVManager* SRVManager);

	//--------- accessor -----------------------------------------------------

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetInputGPUHandle() const { return inputVertex_.srvHandle.second; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetOutputGPUHandle() const { return outputVertex_.uavHandle.second;}
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	InputModelVertex inputVertex_;
	OutputModelVertex outputVertex_;
};