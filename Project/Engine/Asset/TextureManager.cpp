#include "TextureManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Asset/Filesystem.h>
#include <Lib/MathUtils/Algorithm.h>

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
	LOG_INFO("load texture begin: {}", textureName);
	if (textures_.contains(textureName)) {
		LOG_INFO("load texture cached: {}", textureName);
		return;
	}

	// texture検索
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
	if (!found) {

		LOG_WARN("texture not found → {}", textureName);
	}

	std::string identifier = filePath.stem().string();
	TextureData& texture = textures_[identifier];

	// 階層を保存
	std::filesystem::path relative = std::filesystem::relative(filePath, baseDirectoryPath_);
	relative.replace_extension();
	texture.hierarchy = relative.generic_string();

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

	LOG_INFO("load texture ok: {} ({}×{} mip{} srv={})",
		identifier,
		texture.metadata.width,
		texture.metadata.height,
		texture.metadata.mipLevels,
		texture.srvIndex);
}

void TextureManager::LoadLutTexture(const std::string& textureName) {

	// 読みこみ済みなら早期リターン
	if (textures_.contains(textureName)) {
		return;
	}

	// texture検索
	std::filesystem::path filePath;
	bool found = false;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (entry.is_regular_file() &&
			entry.path().stem().string() == textureName) {

			std::string extension = entry.path().extension().string();
			if (extension == ".cube") {

				filePath = entry.path();
				found = true;
				break;
			}
		}
	}
	ASSERT(found, "texture not found in directory or its subdirectories: " + textureName);

	std::string identifier = filePath.stem().string();
	TextureData& texture = textures_[identifier];

	DirectX::ScratchImage mipImages = LoadCubeLUT(filePath);
	texture.metadata = mipImages.GetMetadata();
	CreateTextureResource3D(device_, texture.resource, texture.metadata);
	ComPtr<ID3D12Resource> intermediateResource = nullptr;
	UploadTextureData(texture.resource.Get(), intermediateResource, mipImages);

	// GPU実行の完了を待つ
	dxCommand_->WaitForGPU();
	intermediateResource.Reset();

	// SRV作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(texture.metadata.mipLevels);

	// srv作成
	srvDescriptor_->CreateSRV(texture.srvIndex, texture.resource.Get(), srvDesc);
	texture.gpuHandle = srvDescriptor_->GetGPUHandle(texture.srvIndex);

	std::wstring resourceName = std::wstring(identifier.begin(), identifier.end());
	// debug用にGPUResourceにtextureの名前を設定する
	texture.resource->SetName(resourceName.c_str());

	isCacheValid_ = false;

	SpdLogger::Log("load texture: " + identifier);
}

bool TextureManager::Search(const std::string& textureName) {

	return Algorithm::Find(textures_, textureName);
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

DirectX::ScratchImage TextureManager::LoadCubeLUT(const std::filesystem::path& path) {

	std::ifstream ifs(path);
	if (!ifs) { throw std::runtime_error("open failed"); }

	size_t size = 0;
	std::vector<DirectX::XMFLOAT4> table;

	std::string token;
	while (ifs >> token) {

		//----------------------------
		// キーワード行を先に判定
		//----------------------------
		if (token == "LUT_3D_SIZE") {
			ifs >> size;
			table.reserve(size * size * size);
			continue;
		}
		if (token == "TITLE") {                    // 例: TITLE "Gold"
			std::getline(ifs, token);              // 残り行を読み飛ばす
			continue;
		}
		if (token == "DOMAIN_MIN" || token == "DOMAIN_MAX") {
			float dummy; ifs >> dummy >> dummy >> dummy; // 値は無視
			continue;
		}
		if (token[0] == '#') {                     // コメント
			std::getline(ifs, token);
			continue;
		}

		//----------------------------
		// ここに来たら数値行のはず
		//----------------------------
		float r = std::stof(token);
		float g, b;  ifs >> g >> b;
		table.push_back({ r, g, b, 1.0f });
	}

	if (size == 0 || table.size() != size * size * size) {
		throw std::runtime_error(".cube size mismatch");
	}

	// ScratchImage を作成（ミップ 1, アレイ 1）
	DirectX::ScratchImage img;
	HRESULT hr = img.Initialize3D(DXGI_FORMAT_R16G16B16A16_FLOAT,
		size, size, size, 1);
	assert(SUCCEEDED(hr));

	// DirectXTex は Z→Y→X の順で格納
	size_t index = 0;
	for (size_t z = 0; z < size; ++z) {

		const DirectX::Image* slice = img.GetImage(0, 0, z);
		auto* dst = reinterpret_cast<DirectX::PackedVector::HALF*>(slice->pixels);

		for (size_t y = 0; y < size; ++y)
			for (size_t x = 0; x < size; ++x) {

				const auto& src = table[index++];
				*dst++ = DirectX::PackedVector::XMConvertFloatToHalf(src.x);  // R
				*dst++ = DirectX::PackedVector::XMConvertFloatToHalf(src.y);  // G
				*dst++ = DirectX::PackedVector::XMConvertFloatToHalf(src.z);  // B
				*dst++ = DirectX::PackedVector::XMConvertFloatToHalf(1.0f);   // A
			}
	}
	return img;
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

void TextureManager::CreateTextureResource3D(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, const DirectX::TexMetadata& metadata) {

	HRESULT hr;

	// metadataを元にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};

	resourceDesc.Width = UINT(metadata.width);                  // TextureX
	resourceDesc.Height = UINT(metadata.height);                // TextureY
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // TextureZ

	// TextureのFormat
	resourceDesc.Format = metadata.format;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// 3次元Texture
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

	// mipmapの数
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	// サンプリングカウント。1固定
	resourceDesc.SampleDesc.Count = 1;

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの作成
	hr = device->CreateCommittedResource(
		&heapProperties,                   // Heapの設定
		D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定
		&resourceDesc,                     // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,    // 初回のResourceState、Textureは基本読むだけ
		nullptr,                           // Clear最適値、使わない。
		IID_PPV_ARGS(&resource)            // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
}

[[nodiscard]]
void TextureManager::UploadTextureData(ID3D12Resource* texture, ComPtr<ID3D12Resource>& resource, const DirectX::ScratchImage& mipImages) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	ID3D12GraphicsCommandList* commandlist = dxCommand_->GetCommandList();

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

std::vector<std::string> TextureManager::GetTextureHierarchies() const {

	std::vector<std::string> keys;
	for (const auto& [id, tex] : textures_) {

		keys.push_back(tex.hierarchy);
	}
	return keys;
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