#include "MeshCommandContext.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Mesh/Mesh.h>

//============================================================================
//	MeshCommandContext classMethods
//============================================================================

void MeshCommandContext::DispatchMesh(ID3D12GraphicsCommandList6* commandList,
	UINT instanceCount, uint32_t meshIndex, Mesh* mesh) {

	// 処理するinstanceがない場合は早期リターン
	if (instanceCount == 0) {
		return;
	}

	// buffers
	commandList->SetGraphicsRootShaderResourceView(0,
		mesh->GetVertexBuffer(meshIndex).GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1,
		mesh->GetUniqueVertexIndexBuffer(meshIndex).GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(2,
		mesh->GetMeshletBuffer(meshIndex).GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(3,
		mesh->GetPrimitiveIndexBuffer(meshIndex).GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(5,
		mesh->GetMeshletCountBuffer(meshIndex).GetResource()->GetGPUVirtualAddress());

	// threadGroup数
	UINT threadGroupCountX = mesh->GetMeshletCount(meshIndex) * instanceCount;

	// threadGroupCountXの最大値
	const UINT maxThreadGroupCount = 65535;
	if (threadGroupCountX > maxThreadGroupCount) {

		ASSERT(FALSE, "threadGroupCountX > maxThreadGroupCount");
	}

	// 実行
	commandList->DispatchMesh(threadGroupCountX, 1, 1);
}