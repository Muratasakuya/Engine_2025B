#include "MeshCommandContext.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxCommand.h>

//============================================================================
//	MeshCommandContext classMethods
//============================================================================

void MeshCommandContext::IA(UINT& indexCount, uint32_t meshIndex, const ModelReference& model, DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	if (model.isAnimation) {

		dxCommand->TransitionBarriers({ model.animationModel->GetIOVertex().GetResource() },
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		commandList->IASetVertexBuffers(0, 1,
			&model.animationModel->GetIOVertex().GetVertexBuffer());
		commandList->IASetIndexBuffer(
			&model.animationModel->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
		indexCount = model.animationModel->GetIA().GetIndexCount(meshIndex);
	} else {

		commandList->IASetVertexBuffers(0, 1,
			&model.model->GetIA().GetVertexBuffer(meshIndex).GetVertexBuffer());
		commandList->IASetIndexBuffer(
			&model.model->GetIA().GetIndexBuffer(meshIndex).GetIndexBuffer());
		indexCount = model.model->GetIA().GetIndexCount(meshIndex);
	}
}

void MeshCommandContext::Draw(UINT indexCount, const ModelReference& model, DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	// draw
	commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);

	if (model.isAnimation) {

		dxCommand->TransitionBarriers({ model.animationModel->GetIOVertex().GetResource() },
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}