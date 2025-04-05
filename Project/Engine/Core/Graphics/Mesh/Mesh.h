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

	// 実行処理
	void Dispatch(ID3D12GraphicsCommandList6* commandList);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// マテリアルId
	uint32_t materialId_;
	// meshlet数
	uint32_t meshletCount_;

	// buffers
	DxConstBuffer<MeshVertex> vertices_;
	DxConstBuffer<uint32_t> uniqueVertexIndices_;
	DxConstBuffer<ResourcePrimitiveIndex> primitiveIndices_;
	DxConstBuffer<ResourceMeshlet> meshlets_;
};