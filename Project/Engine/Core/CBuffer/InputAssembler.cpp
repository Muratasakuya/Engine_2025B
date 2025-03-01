#include "InputAssembler.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>

//============================================================================
//	InputAssembler classMethods
//============================================================================

void InputAssembler::Init(const MeshModelData& meshData, ID3D12Device* device) {

	// vertex
	vertices_.emplace_back();
	vertices_.back().CreateVertexBuffer(device, static_cast<UINT>(meshData.vertices.size()));
	vertices_.back().TransferVectorData(meshData.vertices);

	// index
	indices_.emplace_back();
	indexCounts_.emplace_back();
	indices_.back().CreateIndexBuffer(device, static_cast<UINT>(meshData.indices.size()));
	indices_.back().TransferVectorData(meshData.indices);
	indexCounts_.back() = static_cast<UINT>(meshData.indices.size());
}