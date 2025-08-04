#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxObject/DxUploadCommand.h>

// directX
#include <Externals/DirectXTex/DirectXTex.h>
#include <Externals/DirectXTex/d3dx12.h>
#include <DirectXMath.h>
#include <directxpackedvector.h>
// c++
#include <string>
#include <istream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <deque>
#include <thread>
#include <mutex>
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
	~TextureManager();

	void Init(ID3D12Device* device, DxCommand* dxCommand, SRVDescriptor* srvDescriptor);

	void Load(const std::string& textureName);
	void LoadLutTexture(const std::string& textureName);

	bool Search(const std::string& textureName);

	void ReportUsage(bool listAll) const;

	// 非同期処理
	void RequestLoadAsync(const std::string& textureName);
	void WaitAll();

	//--------- accessor -----------------------------------------------------

	// 描画に使用するデータ
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string textureName) const;
	uint32_t GetTextureGPUIndex(const std::string& textureName) const;
	const DirectX::TexMetadata& GetMetaData(const std::string textureName) const;

	std::vector<std::string> GetTextureHierarchies() const;
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
		std::string hierarchy;

		// 使用されたかどうか
		mutable bool isUse = false;
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	DxCommand* dxCommand_;
	SRVDescriptor* srvDescriptor_;

	std::string baseDirectoryPath_;

	std::unordered_map<std::string, TextureData> textures_;

	mutable std::vector<std::string> textureKeysCache_;
	mutable bool isCacheValid_;

	// 非同期処理
	std::unique_ptr<DxUploadCommand> dxUploadCommand_;
	std::thread worker_;
	std::mutex jobMutex_;
	std::condition_variable jobCv_;
	std::deque<std::string> jobs_;
	std::atomic_bool stop_{ false };

	std::mutex gpuMutex_;

	//--------- functions ----------------------------------------------------

	DirectX::ScratchImage GenerateMipMaps(const std::string& filePath);
	DirectX::ScratchImage LoadCubeLUT(const std::filesystem::path& path);

	void CreateTextureResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, const DirectX::TexMetadata& metadata);
	void CreateTextureResource3D(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, const DirectX::TexMetadata& metadata);
	void UploadTextureData(ID3D12Resource* texture, ComPtr<ID3D12Resource>& resource, const DirectX::ScratchImage& mipImages);

	void CreateBufferResource(ID3D12Device* device, ComPtr<ID3D12Resource>& resource, size_t sizeInBytes);
	std::wstring ConvertString(const std::string& str);

	// helper
	bool FindTexturePath(const std::string& textureName, std::filesystem::path& outPath);
	DirectX::ScratchImage DecodeAndGenMips(const std::filesystem::path& filePath, DirectX::TexMetadata& outMeta);

	// 非同期処理
	void WorkerLoop();
	void CreateAndUpload(const std::string& identifier,
		const DirectX::ScratchImage& mipImages, const DirectX::TexMetadata& meta);
};