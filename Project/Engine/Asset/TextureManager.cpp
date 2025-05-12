#include "TextureManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Asset/Filesystem.h>

//============================================================================
//	TextureManager classMethods
//============================================================================

void TextureManager::Init(ID3D12Device* device, DxCommand* dxCommand,
	SRVDescriptor* srvDescriptor) {

	device_ = nullptr;
	device_ = device;

	dxCommand_ = nullptr;
	dxCommand_ = dxCommand;

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	baseDirectoryPath_ = "./Assets/Textures/";
	isCacheValid_ = false;
}

void TextureManager::Load(const std::string& textureName) {

	// 読みこみ済みなら早期リターン
	if (textures_.contains(textureName)) {
		return;
	}

	// model検索
	std::filesystem::path filePath;
	bool found = false;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (entry.is_regular_file() &&
			entry.path().stem().string() == textureName) {

			std::string extension = entry.path().extension().string();
			if (extension == ".png" ||
				extension == ".jpg" ||
				extension == ".dds") {

				filePath = entry.path();
				found = true;
				break;
			}
		}
	}
	ASSERT(found, "texture not found in directory or its subdirectories: " + textureName);

	std::string identifier = filePath.stem().string();
	TextureData& texture = textures_[identifier];

	DirectX::ScratchImage mipImages = GenerateMipMaps(filePath.string());
	texture.metadata = mipImages.GetMetadata();
	CreateTextureResource(device_, texture.resource, texture.metadata);
	ComPtr<ID3D12Resource> intermediateResource = nullptr;
	UploadTextureData(texture.resource.Get(), intermediateResource, mipImages);

	// GPU実行の完了を待つ
	dxCommand_->WaitForGPU();
	intermediateResource.Reset();

	// SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// cubeMap用かチェックして分岐
	if (texture.metadata.IsCubemap()) {

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(texture.metadata.mipLevels);
	}

	// srv作成
	srvDescriptor_->CreateSRV(texture.srvIndex, texture.resource.Get(), srvDesc);
	texture.gpuHandle = srvDescriptor_->GetGPUHandle(texture.srvIndex);

	std::wstring resourceName = std::wstring(identifier.begin(), identifier.end());
	// debug用にGPUResourceにtextureの名前を設定する
	texture.resource->SetName(resourceName.c_str());

	isCacheValid_ = false;

	Logger::Log("load texture: " + identifier);
}

DirectX::ScratchImage TextureManager::GenerateMipMaps(const std::string& filePath) {

	// テクスチャファイルを呼んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	HRESULT hr = S_OK;
	// dds拡張子かどうかで分岐させる
	if (filePathW.ends_with(L".dds")) {

		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
	} else {

		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB | DirectX::WIC_FLAGS_DEFAULT_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}

	DirectX::ScratchImage mipImages{};
	// 圧縮フォーマットかどうかチェックして分岐させる
	if (DirectX::IsCompressed(image.GetMetadata().format)) {

		// 圧縮フォーマットだったらそのまま使用する
		mipImages = std::move(image);
	} else {

		// ミップマップの作成 → 元画像よりも小さなテクスチャ群
		hr = DirectX::GenerateMipMaps(
			image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB, 4, mipImages);
		assert(SUCCEEDED(hr));
	}

	// ミップマップ付きのデータを返す
	return mipImages;
}

void TextureManager::CreateTextureResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, const DirectX::TexMetadata& metadata) {

	HRESULT hr;

	// metadataを元にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);                             // Textureの幅
	resourceDesc.Height = UINT(metadata.height);                           // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);                   // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);            // 奥行き　or 配列textureの配列数
	resourceDesc.Format = metadata.format;                                 // TextureのFormat
	resourceDesc.SampleDesc.Count = 1;                                     // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数、普段は2次元

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの作成
	hr =
		device->CreateCommittedResource(
			&heapProperties,                // Heapの設定
			D3D12_HEAP_FLAG_NONE,           // Heapの特殊な設定
			&resourceDesc,                  // Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState、Textureは基本読むだけ
			nullptr,                        // Clear最適値、使わない。
			IID_PPV_ARGS(&resource)         // 作成するResourceポインタへのポインタ
		);
	assert(SUCCEEDED(hr));
}

[[nodiscard]]
void TextureManager::UploadTextureData(ID3D12Resource* texture, ComPtr<ID3D12Resource>& resource, const DirectX::ScratchImage& mipImages) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	ID3D12GraphicsCommandList* commandlist = dxCommand_->GetCommandList(CommandListType::Graphics);

	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, static_cast<UINT>(subresources.size()));
	CreateBufferResource(device_, resource, intermediateSize);
	UpdateSubresources(commandlist, texture, resource.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());

	// Textureへの転送後は利用できるようにする
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandlist->ResourceBarrier(1, &barrier);
}

void TextureManager::CreateBufferResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, size_t sizeInBytes) {

	HRESULT hr;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// リソースのサイズ、今回はVector4を3頂点分
	vertexResourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	hr = device->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
}

std::wstring TextureManager::ConvertString(const std::string& str) {

	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& TextureManager::GetGPUHandle(const std::string textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {
		ASSERT(FALSE, "not found texture" + textureName);
	}
	return it->second.gpuHandle;
}

uint32_t TextureManager::GetTextureGPUIndex(const std::string& textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {
		ASSERT(FALSE, "not found texture" + textureName);
	}
	return it->second.srvIndex;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {
		ASSERT(FALSE, "not found texture" + textureName);
	}
	TextureData textureData = it->second;
	return it->second.metadata;
}

const std::vector<std::string>& TextureManager::GetTextureKeys() const {

	if (!isCacheValid_) {

		textureKeysCache_.clear();
		textureKeysCache_.reserve(textures_.size());
		for (const auto& pair : textures_) {

			textureKeysCache_.emplace_back(pair.first);
		}
		isCacheValid_ = true;
	}
	return textureKeysCache_;
}