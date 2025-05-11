#include "Mesh.h"

//============================================================================
//	IMesh classMethods
//============================================================================

void IMesh::Init(ID3D12Device* device, const ResourceMesh& resource,
	bool isSkinned, uint32_t numInstance) {

	isSkinned_ = isSkinned;

	// meshの数分
	for (uint32_t meshIndex = 0; meshIndex < resource.meshCount_; ++meshIndex) {

		// buffer生成
		CreateBuffer(device, meshIndex, resource);
		CreateVertexBuffer(device, meshIndex, resource, numInstance);

		// meshlet数
		meshletCounts_.push_back(static_cast<uint32_t>(resource.meshlets[meshIndex].size()));
		// 頂点数
		vertexCounts_.push_back(static_cast<UINT>(resource.vertices[meshIndex].size()));
		// remapデータ
		//remapTables_.push_back(resource.remapTables[meshIndex]);

		// buffer転送
		TransferBuffer(meshIndex, resource, isSkinned);
		TransferVertexBuffer(meshIndex, resource);
	}
}

void IMesh::CreateBuffer(ID3D12Device* device, uint32_t meshIndex,
	const ResourceMesh& resource) {

	// meshInstance情報
	meshInstanceData_.push_back({});
	meshInstanceData_[meshIndex].CreateConstBuffer(device);
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

void IMesh::TransferBuffer(uint32_t meshIndex, const ResourceMesh& resource, bool isSkinned) {

	// meshInstance情報
	meshInstanceData_[meshIndex].TransferData({
		.meshletCount = meshletCounts_[meshIndex],
		.numVertices = static_cast<UINT>(resource.vertices[meshIndex].size()),
		.isSkinned = static_cast<int32_t>(isSkinned) });
	// インデックス
	uniqueVertexIndices_[meshIndex].TransferVectorData(resource.uniqueVertexIndices[meshIndex]);
	// プリミティブ
	primitiveIndices_[meshIndex].TransferVectorData(resource.primitiveIndices[meshIndex]);
	// meshlet
	meshlets_[meshIndex].TransferVectorData(resource.meshlets[meshIndex]);
}

//============================================================================
//	StaticMesh classMethods
//============================================================================

void StaticMesh::CreateVertexBuffer(ID3D12Device* device, uint32_t meshIndex,
	const ResourceMesh& resource, [[maybe_unused]] uint32_t numInstance) {

	// 頂点
	const UINT vertexCount = static_cast<UINT>(resource.vertices[meshIndex].size());
	vertices_.push_back({});
	vertices_[meshIndex].CreateStructuredBuffer(device, vertexCount);
}

void StaticMesh::TransferVertexBuffer(uint32_t meshIndex, const ResourceMesh& resource) {

	// 頂点
	vertices_[meshIndex].TransferVectorData(resource.vertices[meshIndex]);
}

//============================================================================
//	SkinnedMesh classMethods
//============================================================================

void SkinnedMesh::CreateVertexBuffer(ID3D12Device* device, uint32_t meshIndex,
	const ResourceMesh& resource, uint32_t numInstance) {

	// 入力頂点
	const UINT vertexCount = static_cast<UINT>(resource.vertices[meshIndex].size());
	inputVertices_.push_back({});
	inputVertices_[meshIndex].CreateStructuredBuffer(device, vertexCount);
	// 出力頂点
	outputVertices_.push_back({});
	outputVertices_[meshIndex].CreateUavStructuredBuffer(device, vertexCount * numInstance);
}

void SkinnedMesh::TransferVertexBuffer(uint32_t meshIndex, const ResourceMesh& resource) {

	// 入力頂点
	inputVertices_[meshIndex].TransferVectorData(resource.vertices[meshIndex]);
}