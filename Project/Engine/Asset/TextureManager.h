#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/ComPtr.h>

// directX
#include <d3d12.h>
#include <Externals/DirectXTex/DirectXTex.h>
#include <Externals/DirectXTex/d3dx12.h>
// c++
#include <string>
#include <unordered_map>
// front
class DxCommand;
class SRVDescriptor;

//============================================================================
//	TextureManager class
//============================================================================
class TextureManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TextureManager() = default;
	~TextureManager() = default;

	void Init(ID3D12Device* device, DxCommand* dxCommand, SRVDescriptor* srvDescriptor);

	void Load(const std::string& textureName);

	//--------- accessor -----------------------------------------------------

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string textureName) const;

	uint32_t GetTextureGPUIndex(const std::string& textureName) const;

	const DirectX::TexMetadata& GetMetaData(const std::string textureName) const;

	const std::vector<std::string>& GetTextureKeys() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structures ----------------------------------------------------

	struct TextureData {

		ComPtr<ID3D12Resource> resource;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		DirectX::TexMetadata metadata;
		uint32_t srvIndex;
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	DxCommand* dxCommand_;
	SRVDescriptor* srvDescriptor_;

	std::string baseDirectoryPath_;

	std::unordered_map<std::string, TextureData> textures_;

	mutable std::vector<std::string> textureKeysCache_;
	mutable bool isCacheValid_;

	//--------- functions ----------------------------------------------------

	DirectX::ScratchImage GenerateMipMaps(const std::string& filePath);

	void CreateTextureResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, const DirectX::TexMetadata& metadata);
	void UploadTextureData(ID3D12Resource* texture, ComPtr<ID3D12Resource>& resource, const DirectX::ScratchImage& mipImages);

	void CreateBufferResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, size_t sizeInBytes);
	std::wstring ConvertString(const std::string& str);
};