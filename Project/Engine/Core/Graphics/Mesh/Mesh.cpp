#include "Mesh.h"

//============================================================================
//	Mesh classMethods
//============================================================================

void Mesh::Init(ID3D12Device* device, const ResourceMesh& resource) {

	// meshの数分
	for (uint32_t meshIndex = 0; meshIndex < resource.meshCount_; ++meshIndex) {

		// buffer生成
		CreateBuffer(device, meshIndex, resource);

		// meshlet数
		meshletCounts_.push_back(static_cast<uint32_t>(resource.meshlets[meshIndex].size()));
	
		// buffer転送
		TransferBuffer(meshIndex, resource);
	}
}

void Mesh::CreateBuffer(ID3D12Device* device, uint32_t meshIndex,
	const ResourceMesh& resource) {

	// meshlet数
	meshletCountsBuffer_.push_back({});
	meshletCountsBuffer_[meshIndex].CreateConstBuffer(device);
	// 頂点
	const UINT vertexCount = static_cast<UINT>(resource.vertices[meshIndex].size());
	vertices_.push_back({});
	vertices_[meshIndex].CreateStructuredBuffer(device, vertexCount);
	// インデックス
	const UINT uniqueVertexIndexCount = static_cast<UINT>(resource.uniqueVertexIndices[meshIndex].size());
	uniqueVertexIndices_.push_back({});
	uniqueVertexIndices_[meshIndex].CreateStructuredBuffer(device, uniqueVertexIndexCount);
	// プリミティブ
	const UINT primitiveIndexCount = static_cast<UINT>(resource.primitiveIndices[meshIndex].size());
	primitiveIndices_.push_back({});
	primitiveIndices_[meshIndex].CreateStructuredBuffer(device, primitiveIndexCount);
	// meshlet
	const UINT meshletCount = static_cast<UINT>(resource.meshlets[meshIndex].size());
	meshlets_.push_back({});
	meshlets_[meshIndex].CreateStructuredBuffer(device, meshletCount);
}

void Mesh::TransferBuffer(uint32_t meshIndex,const ResourceMesh& resource) {

	// meshlet数
	meshletCountsBuffer_[meshIndex].TransferData(meshletCounts_[meshIndex]);
	// 頂点
	vertices_[meshIndex].TransferVectorData(resource.vertices[meshIndex]);
	// インデックス
	uniqueVertexIndices_[meshIndex].TransferVectorData(resource.uniqueVertexIndices[meshIndex]);
	// プリミティブ
	primitiveIndices_[meshIndex].TransferVectorData(resource.primitiveIndices[meshIndex]);
	// meshlet
	meshlets_[meshIndex].TransferVectorData(resource.meshlets[meshIndex]);
}