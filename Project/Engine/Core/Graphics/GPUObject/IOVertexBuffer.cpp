#include "IOVertexBuffer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>

//============================================================================
//	IOVertexBuffer classMethods
//============================================================================

void IOVertexBuffer::Init(UINT vertexNum, ID3D12Resource* vertexResource,
	ID3D12Device* device, SRVDescriptor* srvDescriptor) {

	// inputVertexData
	// SRVDesc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = vertexNum;
	srvDesc.Buffer.StructureByteStride = static_cast<UINT>(sizeof(MeshVertex));
	// SRV作成
	srvDescriptor->CreateSRV(inputVertex_.srvIndex, vertexResource, srvDesc);
	inputVertex_.srvHandle.second = srvDescriptor->GetGPUHandle(inputVertex_.srvIndex);

	// outputVertexData
	DxConstBuffer::CreateUavVertexBuffer(device, vertexNum);
	// UAVDesc設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.NumElements = vertexNum;
	uavDesc.Buffer.StructureByteStride = static_cast<UINT>(sizeof(MeshVertex));
	// UAV作成
	srvDescriptor->CreateUAV(outputVertex_.uavIndex, this->GetResource(), uavDesc);
	outputVertex_.uavHandle.second = srvDescriptor->GetGPUHandle(outputVertex_.uavIndex);
}