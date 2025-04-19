#include "PrimitiveMesh.h"

//============================================================================
//	PrimitiveMesh classMethods
//============================================================================

void PrimitiveMesh::Init(ID3D12Device* device, const ResourceMesh& resource) {

	// meshlet数
	meshletCount_ = static_cast<uint32_t>(resource.meshlets.front().size());

	// buffer生成
	CreateBuffer(device, resource);

	// buffer転送
	TransferBuffer(resource);
}

void PrimitiveMesh::Update(const ResourceMesh& resource) {

	// buffer転送
	// 頂点
	vertices_.TransferVectorData(resource.vertices.front());
}

void PrimitiveMesh::CreateBuffer(ID3D12Device* device, const ResourceMesh& resource) {

	// multiMeshは作らないので、vector<T>.front()で処理を行う

	// 頂点
	const UINT vertexCount = static_cast<UINT>(resource.vertices.front().size());
	vertices_.CreateStructuredBuffer(device, vertexCount);
	// インデックス
	const UINT uniqueVertexIndexCount = static_cast<UINT>(resource.uniqueVertexIndices.front().size());
	uniqueVertexIndices_.CreateStructuredBuffer(device, uniqueVertexIndexCount);
	// プリミティブ
	const UINT primitiveIndexCount = static_cast<UINT>(resource.primitiveIndices.front().size());
	primitiveIndices_.CreateStructuredBuffer(device, primitiveIndexCount);
	// meshlet
	const UINT meshletCount = static_cast<UINT>(resource.meshlets.front().size());
	meshlet_.CreateStructuredBuffer(device, meshletCount);
}

void PrimitiveMesh::TransferBuffer(const ResourceMesh& resource) {

	// multiMeshは作らないので、vector<T>.front()で処理を行う

	// 頂点
	vertices_.TransferVectorData(resource.vertices.front());
	// インデックス
	uniqueVertexIndices_.TransferVectorData(resource.uniqueVertexIndices.front());
	// プリミティブ
	primitiveIndices_.TransferVectorData(resource.primitiveIndices.front());
	// meshlet
	meshlet_.TransferVectorData(resource.meshlets.front());
}