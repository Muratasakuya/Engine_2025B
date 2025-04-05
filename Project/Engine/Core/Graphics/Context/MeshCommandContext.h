#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/ModelComponent.h>

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

	void IA(UINT& indexCount, uint32_t meshIndex,
		const ModelReference& model, DxCommand* dxCommand);
	void IA(UINT& indexCount, uint32_t meshIndex,
		const InstancingModelReference& model, DxCommand* dxCommand);

	void Draw(UINT indexCount, const ModelReference& model, DxCommand* dxCommand);
	void InstancingDraw(UINT indexCount, uint32_t numInstance, ID3D12GraphicsCommandList* commandList);
};