#include "ModelComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/Managers/RenderObjectManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	Model classMethods
//============================================================================

void BaseModel::Init(const std::string& modelName, ID3D12Device* device, Asset* asset) {

	asset_ = asset;

	modelData_ = asset->GetModelData(modelName);
	meshNum_ = modelData_.meshes.size();

	for (uint32_t index = 0; index < meshNum_; ++index) {

		// 強制Textute貼り付け
		if (!modelData_.meshes[index].textureName.has_value()) {
			modelData_.meshes[index].textureName = "white";
		}

		inputAssembler_.Init(modelData_.meshes[index], device);
	}
}

void BaseModel::SetTexture(const std::string& textureName, const std::optional<uint32_t>& meshIndex) {

	if (meshIndex.has_value()) {

		modelData_.meshes[*meshIndex].textureName = textureName;
	} else {
		for (uint32_t index = 0; index < meshNum_; ++index) {

			modelData_.meshes[index].textureName = textureName;
		}
	}
}

const D3D12_GPU_DESCRIPTOR_HANDLE& BaseModel::GetTextureGPUHandle(uint32_t meshIndex) {
	return asset_->GetGPUHandle(*modelData_.meshes[meshIndex].textureName);
}

uint32_t BaseModel::GetTextureGPUIndex(uint32_t meshIndex) const {
	return asset_->GetTextureGPUIndex(*modelData_.meshes[meshIndex].textureName);
}

//============================================================================
//	AnimationModel classMethods
//============================================================================

void AnimationModel::Init(const std::string& modelName, const std::string& animationName,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Asset* asset, class SRVManager* srvManager) {

	asset_ = nullptr;
	asset_ = asset;

	commandList_ = nullptr;
	commandList_ = commandList;

	animationName_ = animationName;

	BaseModel::Init(modelName, device, asset);
	UINT vertexNum = static_cast<uint32_t>(BaseModel::GetModelData().meshes.front().vertices.size());

	ioVertexBuffer_.Init(vertexNum, inputAssembler_.GetVertexBuffer(0).GetResource(),
		device, srvManager);
	skinningInfoDates_.CreateConstBuffer(device, 4);
	skinningInfoDates_.TransferData(static_cast<uint32_t>(BaseModel::GetModelData().meshes.front().vertices.size()));
}

void AnimationModel::Skinning() {

	commandList_->SetComputeRootDescriptorTable(0,
		asset_->GetSkinClusterData(animationName_).paletteSrvHandle.second);
	commandList_->SetComputeRootDescriptorTable(1, ioVertexBuffer_.GetInputGPUHandle());
	commandList_->SetComputeRootDescriptorTable(2,
		asset_->GetSkinClusterData(animationName_).influenceSrvHandle.second);
	commandList_->SetComputeRootDescriptorTable(3, ioVertexBuffer_.GetOutputGPUHandle());
	commandList_->SetComputeRootConstantBufferView(4, skinningInfoDates_.GetResource()->GetGPUVirtualAddress());
	commandList_->Dispatch(static_cast<UINT>(modelData_.meshes.front().vertices.size() + 1023) / 1024, 1, 1);
}

//============================================================================
//	ModelComponent ImGui
//============================================================================

void ModelComponent::ImGui() {

	if (isAnimation) {

		ImGui::Text("meshCount: %d", animationModel->GetMeshNum());
	} else {

		ImGui::Text("meshCount: %d", model->GetMeshNum());
	}

	// 描画しないかするかの有無
	ImGui::Checkbox("drawEnable", &renderingData.drawEnable);
}