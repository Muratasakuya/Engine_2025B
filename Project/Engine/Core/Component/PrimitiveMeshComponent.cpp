#include "PrimitiveMeshComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/Mesh/MeshletBuilder.h>

// imgui
#include <imgui.h>

//============================================================================
//	PrimitiveMeshComponent classMethods
//============================================================================

void PrimitiveMeshComponent::Init(ID3D12Device* device, Asset* asset,
	const std::string& modelName) {

	asset_ = nullptr;
	asset_ = asset;

	// 頂点、meshlet生成
	resourceMesh_ = CreateMeshlet(modelName);

	// buffer作成
	primitiveMesh_ = std::make_unique<PrimitiveMesh>();
	primitiveMesh_->Init(device, *resourceMesh_.get());
}

void PrimitiveMeshComponent::Update() {

	// buffer更新
	primitiveMesh_->Update(*resourceMesh_.get());
}

void PrimitiveMeshComponent::ImGui(float itemWidth) {

	// 横幅設定
	ImGui::PushItemWidth(itemWidth);

	// blendModeの選択
	SelectBlendMode();

	ImGui::PopItemWidth();
}

void PrimitiveMeshComponent::SelectBlendMode() {

	const std::vector<std::string> blendModeNames = {
		"ModeNormal",
		"ModeAdd",
		"ModeSubtract",
		"ModeMultiply",
		"ModeScreen",
	};

	// primitiveの選択
	if (ImGui::BeginCombo("BlendMode", blendModeNames[selectBlendModeIndex_].c_str())) {
		for (int index = 0; index < blendModeNames.size(); ++index) {

			const bool isSelected = (selectBlendModeIndex_ == index);
			if (ImGui::Selectable(blendModeNames[index].c_str(), isSelected)) {

				// 追加する予定のmodelを設定
				selectBlendModeIndex_ = index;
				primitiveMesh_->SetBlendMode(static_cast<BlendMode>(selectBlendModeIndex_));
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

std::unique_ptr<ResourceMesh> PrimitiveMeshComponent::CreateMeshlet(const std::string& modelName) {

	MeshletBuilder meshletBuilder{};

	Assimp::Importer importer;
	ModelData modelData = asset_->GetModelData(modelName);

	// 頂点、meshlet生成
	std::unique_ptr<ResourceMesh> resourceMeshPtr = std::make_unique<ResourceMesh>();
	*resourceMeshPtr = meshletBuilder.ParseMesh(modelData);

	return std::move(resourceMeshPtr);
}