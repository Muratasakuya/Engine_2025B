#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/CBuffer/DxConstBuffer.h>
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>

//============================================================================
//	Mesh class
//============================================================================
class Mesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Mesh() = default;
	~Mesh() = default;

	// 初期化
	void Init(ID3D12Device* device, const ResourceMesh& resource);

	//--------- accessor -----------------------------------------------------

	// mesh数
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(meshletCounts_.size()); }

	// meshlet数
	uint32_t GetMeshletCount(uint32_t meshIndex) const { return meshletCounts_[meshIndex]; }

	const DxConstBuffer<uint32_t>& GetMeshletCountBuffer(uint32_t meshIndex) const { return meshletCountsBuffer_[meshIndex]; }

	const DxConstBuffer<MeshVertex>& GetVertexBuffer(uint32_t meshIndex) const { return vertices_[meshIndex]; }

	const DxConstBuffer<uint32_t>& GetUniqueVertexIndexBuffer(uint32_t meshIndex) const { return uniqueVertexIndices_[meshIndex]; }

	const DxConstBuffer<ResourcePrimitiveIndex>& GetPrimitiveIndexBuffer(uint32_t meshIndex) const { return primitiveIndices_[meshIndex]; }

	const DxConstBuffer<ResourceMeshlet>& GetMeshletBuffer(uint32_t meshIndex) const { return meshlets_[meshIndex]; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// meshlet数
	std::vector<uint32_t> meshletCounts_;

	// buffers
	std::vector<DxConstBuffer<uint32_t>> meshletCountsBuffer_;
	std::vector<DxConstBuffer<MeshVertex>> vertices_;
	std::vector<DxConstBuffer<uint32_t>> uniqueVertexIndices_;
	std::vector<DxConstBuffer<ResourcePrimitiveIndex>> primitiveIndices_;
	std::vector<DxConstBuffer<ResourceMeshlet>> meshlets_;

	//--------- functions ----------------------------------------------------

	void CreateBuffer(ID3D12Device* device, uint32_t meshIndex, const ResourceMesh& resource);
	void TransferBuffer(uint32_t meshIndex, const ResourceMesh& resource);
};