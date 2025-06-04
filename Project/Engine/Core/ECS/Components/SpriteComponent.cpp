#include "SpriteComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>

// imgui
#include <imgui.h>

//============================================================================
//	SpriteComponent classMethods
//============================================================================

SpriteComponent::SpriteComponent(ID3D12Device* device, Asset* asset,
	const std::string& textureName, Transform2DComponent& transform) {

	asset_ = nullptr;
	asset_ = asset;

	textureName_ = textureName;

	layer_ = SpriteLayer::PostModel;
	postProcessEnable_ = true;

	// buffer作成
	InitBuffer(device);

	// textureSizeにtransformを合わせる
	SetMetaDataTextureSize(transform);
}

void SpriteComponent::UpdateVertex(const Transform2DComponent& transform) {

	float left = 0.0f - transform.anchorPoint.x;
	float right = 1.0f - transform.anchorPoint.x;
	float top = 0.0f - transform.anchorPoint.y;
	float bottom = 1.0f - transform.anchorPoint.y;

	// textureMetadataの取得
	const DirectX::TexMetadata& metadata = asset_->GetMetaData(textureName_);

	// 横
	float texLeft = transform.textureLeftTop.x / static_cast<float>(metadata.width);
	float texRight = (transform.textureLeftTop.x + transform.textureSize.x) / static_cast<float>(metadata.width);
	// 縦
	float texTop = transform.textureLeftTop.y / static_cast<float>(metadata.height);
	float texBottom = (transform.textureLeftTop.y + transform.textureSize.y) / static_cast<float>(metadata.height);

	// vertexデータの更新
	// 左下
	vertexData_[0].pos = { left,bottom };
	vertexData_[0].texcoord = { texLeft,texBottom };
	vertexData_[0].color = Color::White();
	// 左上
	vertexData_[1].pos = { left,top };
	vertexData_[1].texcoord = { texLeft,texTop };
	vertexData_[1].color = Color::White();
	// 右下
	vertexData_[2].pos = { right,bottom };
	vertexData_[2].texcoord = { texRight,texBottom };
	vertexData_[2].color = Color::White();
	// 右上
	vertexData_[3].pos = { right,top };
	vertexData_[3].texcoord = { texRight,texTop };
	vertexData_[3].color = Color::White();

	// GPUデータ転送
	vertexBuffer_.TransferVectorData(vertexData_);
}

void SpriteComponent::InitBuffer(ID3D12Device* device) {

	// buffer作成
	vertexBuffer_.CreateVertexBuffer(device, kVertexNum_);
	indexBuffer_.CreateIndexBuffer(device, kIndexNum_);

	// vertexデータの初期化
	vertexData_.resize(kVertexNum_);

	// indexデータの初期化
	std::vector<uint32_t> indexData(kIndexNum_);

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;

	// GPUデータ転送
	indexBuffer_.TransferVectorData(indexData);
}

void SpriteComponent::SetMetaDataTextureSize(Transform2DComponent& transform) {

	float left = 0.0f - transform.anchorPoint.x;
	float right = 1.0f - transform.anchorPoint.x;
	float top = 0.0f - transform.anchorPoint.y;
	float bottom = 1.0f - transform.anchorPoint.y;

	// textureMetadataの取得
	const DirectX::TexMetadata& metadata = asset_->GetMetaData(textureName_);

	// textureSizeの設定
	transform.textureSize = { static_cast<float>(metadata.width) ,static_cast<float>(metadata.height) };
	transform.size = transform.textureSize;

	// 横
	float texLeft = transform.textureLeftTop.x / static_cast<float>(metadata.width);
	float texRight = (transform.textureLeftTop.x + transform.textureSize.x) / static_cast<float>(metadata.width);
	// 縦
	float texTop = transform.textureLeftTop.y / static_cast<float>(metadata.height);
	float texBottom = (transform.textureLeftTop.y + transform.textureSize.y) / static_cast<float>(metadata.height);

	// vertexデータの初期化
	// 左下
	vertexData_[0].pos = { left,bottom };
	vertexData_[0].texcoord = { texLeft,texBottom };
	// 左上
	vertexData_[1].pos = { left,top };
	vertexData_[1].texcoord = { texLeft,texTop };
	// 右下
	vertexData_[2].pos = { right,bottom };
	vertexData_[2].texcoord = { texRight,texBottom };
	// 右上
	vertexData_[3].pos = { right,top };
	vertexData_[3].texcoord = { texRight,texTop };
}


void SpriteComponent::ImGui(float itemSize) {

	if (ImGui::Checkbox("postProcessEnable", &postProcessEnable_)) {

		if (!postProcessEnable_) {

			// postProcessをかけないとき、
			// frameBufferに直接描画するのでmodelの後になる
			layer_ = SpriteLayer::PostModel;
		}
	}

	ImGui::PushItemWidth(itemSize);
	const char* layerItems[] = {
		"PreModel",
		"PostModel",
	};
	int layerIndex = static_cast<int>(layer_);

	if (ImGui::Combo("SpriteLayer", &layerIndex, layerItems, IM_ARRAYSIZE(layerItems))) {

		layer_ = static_cast<SpriteLayer>(layerIndex);
	}
	ImGui::PopItemWidth();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SpriteComponent::GetTextureGPUHandle() const {

	return asset_->GetGPUHandle(textureName_);
}