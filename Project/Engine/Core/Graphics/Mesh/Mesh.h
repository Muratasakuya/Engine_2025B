#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>
#include <Engine/Core/Graphics/GPUObject/IOVertexBuffer.h>

//============================================================================
//	IMesh class
//============================================================================
class IMesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IMesh() = default;
	virtual ~IMesh() = default;

	void Init(ID3D12Device* device, const ResourceMesh& resource,
		bool isSkinned, uint32_t numInstance);

	//--------- accessor -----------------------------------------------------

	bool IsSkinned() const { return isSkinned_; }

	// mesh数
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(meshletCounts_.size()); }

	// meshlet数
	uint32_t GetMeshletCount(uint32_t meshIndex) const { return meshletCounts_[meshIndex]; }
	uint32_t GetVertexCount(uint32_t meshIndex) const { return vertexCounts_[meshIndex]; }

	// 頂点のremapデータ
	const std::vector<uint32_t>& GetRemapData(uint32_t meshIndex) { return remapTables_[meshIndex]; }

	const DxConstBuffer<MeshInstanceData>& GetMeshInstanceData(uint32_t meshIndex) const { return meshInstanceData_[meshIndex]; }

	const DxConstBuffer<uint32_t>& GetUniqueVertexIndexBuffer(uint32_t meshIndex) const { return uniqueVertexIndices_[meshIndex]; }

	const DxConstBuffer<ResourcePrimitiveIndex>& GetPrimitiveIndexBuffer(uint32_t meshIndex) const { return primitiveIndices_[meshIndex]; }

	const DxConstBuffer<ResourceMeshlet>& GetMeshletBuffer(uint32_t meshIndex) const { return meshlets_[meshIndex]; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// staticかskinnedかのフラグ
	bool isSkinned_;

	// meshlet数
	std::vector<uint32_t> meshletCounts_;
	// 頂点数
	std::vector<uint32_t> vertexCounts_;

	// 頂点のremapデータ
	std::vector<std::vector<uint32_t>> remapTables_;

	// buffers
	std::vector<DxConstBuffer<MeshInstanceData>> meshInstanceData_;
	std::vector<DxConstBuffer<uint32_t>> uniqueVertexIndices_;
	std::vector<DxConstBuffer<ResourcePrimitiveIndex>> primitiveIndices_;
	std::vector<DxConstBuffer<ResourceMeshlet>> meshlets_;

	//--------- functions ----------------------------------------------------

	void CreateBuffer(ID3D12Device* device, uint32_t meshIndex,
		const ResourceMesh& resource);
	void TransferBuffer(uint32_t meshIndex, const ResourceMesh& resource,
		bool isSkinned);

	virtual void CreateVertexBuffer(ID3D12Device* device, uint32_t meshIndex,
		const ResourceMesh& resource, uint32_t numInstance) = 0;
	virtual void TransferVertexBuffer(uint32_t meshIndex, const ResourceMesh& resource) = 0;
};

//============================================================================
//	StaticMesh class
//============================================================================
class StaticMesh :
	public IMesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	StaticMesh() = default;
	~StaticMesh() = default;

	//--------- accessor -----------------------------------------------------

	const DxConstBuffer<MeshVertex>& GetVertexBuffer(uint32_t meshIndex) const { return vertices_[meshIndex]; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// vertexBuffer
	std::vector<DxConstBuffer<MeshVertex>> vertices_;

	//--------- functions ----------------------------------------------------

	void CreateVertexBuffer(ID3D12Device* device, uint32_t meshIndex,
		const ResourceMesh& resource, uint32_t numInstance) override;
	void TransferVertexBuffer(uint32_t meshIndex, const ResourceMesh& resource) override;
};

//============================================================================
//	SkinnedMesh class
//============================================================================
class SkinnedMesh :
	public IMesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SkinnedMesh() = default;
	~SkinnedMesh() = default;

	//--------- accessor -----------------------------------------------------

	const DxConstBuffer<MeshVertex>& GetInputVertexBuffer(uint32_t meshIndex) const { return inputVertices_[meshIndex]; }
	const DxConstBuffer<MeshVertex>& GetOutputVertexBuffer(uint32_t meshIndex) const { return outputVertices_[meshIndex]; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// vertexBuffer
	std::vector<DxConstBuffer<MeshVertex>> inputVertices_;
	std::vector<DxConstBuffer<MeshVertex>> outputVertices_;

	//--------- functions ----------------------------------------------------

	void CreateVertexBuffer(ID3D12Device* device, uint32_t meshIndex,
		const ResourceMesh& resource, uint32_t numInstance) override;
	void TransferVertexBuffer(uint32_t meshIndex, const ResourceMesh& resource) override;
};