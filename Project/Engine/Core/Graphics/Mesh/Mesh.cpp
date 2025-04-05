#include "Mesh.h"

//============================================================================
//	Mesh classMethods
//============================================================================

void Mesh::Init(ID3D12Device* device, const ResourceMesh& resource) {

	//------------------------------------------------------------------------
	// buffer初期化

	// 頂点
	const UINT vertexCount = static_cast<UINT>(resource.vertices.size());
	vertices_.CreateStructuredBuffer(device, vertexCount);
	// インデックス
	const UINT uniqueVertexIndexCount = static_cast<UINT>(resource.uniqueVertexIndices.size());
	uniqueVertexIndices_.CreateStructuredBuffer(device, uniqueVertexIndexCount);
	// プリミティブ
	const UINT primitiveIndexCount = static_cast<UINT>(resource.primitiveIndices.size());
	primitiveIndices_.CreateStructuredBuffer(device, primitiveIndexCount);
	// meshlet
	const UINT meshletCount = static_cast<UINT>(resource.meshlets.size());
	meshlets_.CreateStructuredBuffer(device, meshletCount);

	// マテリアルId
	materialId_ = resource.materialId;

	// meshlet数
	meshletCount_ = static_cast<uint32_t>(resource.meshlets.size());

	//------------------------------------------------------------------------
	// buffer転送

	// 頂点
	vertices_.TransferVectorData(resource.vertices);
	// インデックス
	uniqueVertexIndices_.TransferVectorData(resource.uniqueVertexIndices);
	// プリミティブ
	primitiveIndices_.TransferVectorData(resource.primitiveIndices);
	// meshlet
	meshlets_.TransferVectorData(resource.meshlets);
}

void Mesh::Dispatch(ID3D12GraphicsCommandList6* commandList) {

	// buffers
	// vertex
	commandList->SetGraphicsRootShaderResourceView(0,
		vertices_.GetResource()->GetGPUVirtualAddress());
	// uniqueVertexIndices
	commandList->SetGraphicsRootShaderResourceView(1,
		uniqueVertexIndices_.GetResource()->GetGPUVirtualAddress());
	// meshlets
	commandList->SetGraphicsRootShaderResourceView(2,
		meshlets_.GetResource()->GetGPUVirtualAddress());
	// primitiveIndices
	commandList->SetGraphicsRootShaderResourceView(3,
		primitiveIndices_.GetResource()->GetGPUVirtualAddress());

	// 実行
	commandList->DispatchMesh(meshletCount_, 1, 1);
}