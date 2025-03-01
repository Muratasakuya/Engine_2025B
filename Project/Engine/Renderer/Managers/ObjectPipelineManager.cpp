#include "ObjectPipelineManager.h"

//============================================================================
//	ObjectPipelineManager classMethods
//============================================================================

void ObjectPipelineManager::Create(ID3D12GraphicsCommandList* commandList,
	ID3D12Device* device, DxShaderCompiler* shaderCompiler) {

	commandList_ = nullptr;
	commandList_ = commandList;

	object3DPipeline_ = std::make_unique<PipelineState>();
	object3DPipeline_->Create("Object3D.json", device, shaderCompiler);

	zPassPipeline_ = std::make_unique<PipelineState>();
	zPassPipeline_->Create("ZPass.json", device, shaderCompiler);
}

void ObjectPipelineManager::SetObjectPipeline(BlendMode blendMode) {

	commandList_->SetGraphicsRootSignature(object3DPipeline_->GetRootSignature());
	commandList_->SetPipelineState(object3DPipeline_->GetGraphicsPipeline(blendMode));
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ObjectPipelineManager::SetZPassPipeline() {

	commandList_->SetGraphicsRootSignature(zPassPipeline_->GetRootSignature());
	commandList_->SetPipelineState(zPassPipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}