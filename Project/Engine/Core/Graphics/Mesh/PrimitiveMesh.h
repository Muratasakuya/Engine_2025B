#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>

//============================================================================
//	PrimitiveMesh class
//============================================================================
class PrimitiveMesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveMesh() = default;
	~PrimitiveMesh() = default;

	void Init(ID3D12Device* device, const ResourceMesh& resource);

	//--------- accessor -----------------------------------------------------

	// meshlet数
	uint32_t GetMeshletCount() const { return meshletCount_; }

	const DxConstBuffer<MeshVertex>& GetVertexBuffer() const { return vertices_; }

	const DxConstBuffer<uint32_t>& GetUniqueVertexIndexBuffer() const { return uniqueVertexIndices_; }

	const DxConstBuffer<ResourcePrimitiveIndex>& GetPrimitiveIndexBuffer() const { return primitiveIndices_; }

	const DxConstBuffer<ResourceMeshlet>& GetMeshletBuffer() const { return meshlet_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// meshlet数
	uint32_t meshletCount_;

	// buffers
	DxConstBuffer<MeshVertex> vertices_;
	DxConstBuffer<uint32_t> uniqueVertexIndices_;
	DxConstBuffer<ResourcePrimitiveIndex> primitiveIndices_;
	DxConstBuffer<ResourceMeshlet> meshlet_;

	//--------- functions ----------------------------------------------------

	void CreateBuffer(ID3D12Device* device, const ResourceMesh& resource);
	void TransferBuffer(const ResourceMesh& resource);
};