#pragma once

//============================================================================
//	include
//============================================================================

// directX
#include <d3d12.h>
// c++
#include <cstdint>
// front
class DxCommand;

//============================================================================
//	MeshCommand class
//============================================================================
class MeshCommandContext {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshCommandContext() = default;
	~MeshCommandContext() = default;

	// instancing
	void DispatchMesh(ID3D12GraphicsCommandList6* commandList,
		UINT instanceCount, uint32_t meshIndex, class IMesh* mesh);
};