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

	instancingObject3DPipeline_ = std::make_unique<PipelineState>();
	instancingObject3DPipeline_->Create("InstancingObject3D.json", device, shaderCompiler);

	zPassPipeline_ = std::make_unique<PipelineState>();
	zPassPipeline_->Create("ZPass.json", device, shaderCompiler);

	instancingZPassPipeline_ = std::make_unique<PipelineState>();
	instancingZPassPipeline_->Create("InstancingObjectShadowDepth.json", device, shaderCompiler);
}

void ObjectPipelineManager::SetObjectPipeline() {

	commandList_->SetGraphicsRootSignature(object3DPipeline_->GetRootSignature());
	commandList_->SetPipelineState(object3DPipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ObjectPipelineManager::SetInstancingObjectPipeline() {

	commandList_->SetGraphicsRootSignature(instancingObject3DPipeline_->GetRootSignature());
	commandList_->SetPipelineState(instancingObject3DPipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ObjectPipelineManager::SetZPassPipeline() {

	commandList_->SetGraphicsRootSignature(zPassPipeline_->GetRootSignature());
	commandList_->SetPipelineState(zPassPipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ObjectPipelineManager::SetInstancingZPassPipeline() {

	commandList_->SetGraphicsRootSignature(instancingZPassPipeline_->GetRootSignature());
	commandList_->SetPipelineState(instancingZPassPipeline_->GetGraphicsPipeline());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}