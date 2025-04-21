#define NOMINMAX

#include "MeshCommandContext.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Mesh/Mesh.h>
#include <Engine/Core/Graphics/Mesh/PrimitiveMesh.h>

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

	// threadGroupCountXの最大値
	const UINT maxThreadGroupCount = 65535;
	const UINT meshletCount = mesh->GetMeshletCount(meshIndex);

	// threadGroup数
	UINT totalThreadGroupCountX = meshletCount * instanceCount;

	// 最大数を超過した場合、分割して実行
	if (totalThreadGroupCountX > maxThreadGroupCount) {

		// 1度に実行できる最大instance数を計算
		UINT maxInstancesPerDispatch = maxThreadGroupCount / meshletCount;

		// 分割して実行
		UINT remainingInstances = instanceCount;
		while (remainingInstances > 0) {

			UINT dispatchInstanceCount = std::min(remainingInstances, maxInstancesPerDispatch);
			UINT dispatchThreadGroupCountX = dispatchInstanceCount * meshletCount;

			// 実行
			commandList->DispatchMesh(dispatchThreadGroupCountX, 1, 1);

			// 残りのinstance数を更新
			remainingInstances -= dispatchInstanceCount;
		}
	}
	// 超過しない場合はそのまま実行
	else {
		commandList->DispatchMesh(totalThreadGroupCountX, 1, 1);
	}
}

void MeshCommandContext::DispatchMesh(ID3D12GraphicsCommandList6* commandList, PrimitiveMesh* mesh) {

	// buffers
	commandList->SetGraphicsRootShaderResourceView(0,
		mesh->GetVertexBuffer().GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1,
		mesh->GetUniqueVertexIndexBuffer().GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(2,
		mesh->GetMeshletBuffer().GetResource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(3,
		mesh->GetPrimitiveIndexBuffer().GetResource()->GetGPUVirtualAddress());

	// threadGroup数
	UINT threadGroupCountX = mesh->GetMeshletCount() ;

	// threadGroupCountXの最大値
	const UINT maxThreadGroupCount = 65535;
	if (threadGroupCountX > maxThreadGroupCount) {

		ASSERT(FALSE, "threadGroupCountX > maxThreadGroupCount");
	}

	// 実行
	commandList->DispatchMesh(threadGroupCountX, 1, 1);
}