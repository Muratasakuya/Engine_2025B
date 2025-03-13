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

//============================================================================
//	AnimationModel classMethods
//============================================================================

void AnimationModel::Init(const std::string& modelName, const std::string& animationName,
	ID3D12Device* device, Asset* asset, class SRVManager* srvManager) {

	animationName_ = animationName;

	BaseModel::Init(modelName, device, asset);
	UINT vertexNum = static_cast<uint32_t>(BaseModel::GetModelData().meshes.front().vertices.size());

	ioVertexBuffer_.Init(vertexNum, inputAssembler_.GetVertexBuffer(0).GetResource(),
		device, srvManager);
	skinningInfoDates_.CreateConstBuffer(device);
	skinningInfoDates_.TransferData(static_cast<uint32_t>(BaseModel::GetModelData().meshes.front().vertices.size()));
}

//============================================================================
//	ModelComponent ImGui
//============================================================================

void ModelComponent::ImGui(float itemSize, RenderObjectManager* renderObjectManager) {

	if (isAnimation) {

		ImGui::Text("meshCount: %d", animationModel->GetMeshNum());
	} else {

		ImGui::Text("meshCount: %d", model->GetMeshNum());
	}

	// 描画しないかするかの有無
	if (ImGui::Checkbox("drawEnable", &renderingData.drawEnable)) {

		renderObjectManager->SetNeedSorting();
	}

	const char* blendModeItems[] = {
			"Normal",     // kBlendModeNormal
			"Add",        // kBlendModeAdd
			"Subtract",   // kBlendModeSubtract
			"Multiply",   // kBlendModeMultiply
			"Screen"      // kBlendModeScreen
	};
	int blendIndex = static_cast<int>(renderingData.blendMode);

	ImGui::PushItemWidth(itemSize);
	if (ImGui::Combo("blendMode", &blendIndex, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {

		renderingData.blendMode = static_cast<BlendMode>(blendIndex);
		renderObjectManager->SetNeedSorting();
	}
	ImGui::PopItemWidth();
}