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

TextureManager::~TextureManager() {

	stop_ = true;
	jobCv_.notify_all();
	if (worker_.joinable()) worker_.join();
}

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

	// 転送用
	dxUploadCommand_ = std::make_unique<DxUploadCommand>();
	dxUploadCommand_->Create(device_);

	// ワーカースレッド起動
	stop_ = false;
	worker_ = std::thread([this] { this->WorkerLoop(); });
}

void TextureManager::Load(const std::string& textureName) {

	RequestLoadAsync(textureName);
	// 単純同期：当該テクスチャが出来るまでポーリング（プロダクションでは条件変数等で個別待ち推奨）
	for (;;) {
		{
			std::scoped_lock lk(gpuMutex_);
			if (textures_.contains(textureName)) break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
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
	texture.isUse = false;

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

void TextureManager::ReportUsage(bool listAll) const {

	// ロード済みだが未使用の場合のログ出力
	std::vector<std::string> unused;
	unused.reserve(textures_.size());
	for (const auto& [name, tex] : textures_) {
		if (!tex.isUse) {

			unused.emplace_back(name);
		}
	}

	if (unused.empty()) {

		LOG_ASSET_INFO("[Texture] Unused: 0");
	} else {

		LOG_ASSET_INFO("[Texture] Unused: {}", unused.size());
		if (listAll) {
			for (auto& n : unused) {

				LOG_ASSET_INFO("  - {}", n);
			}
		}
	}

	// フォルダ内にあるにも関わらず未使用
	std::unordered_set<std::string> onDisk;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (!entry.is_regular_file()) {
			continue;
		}
		const auto ext = entry.path().extension().string();
		if (ext == ".png" || ext == ".jpg" || ext == ".dds" || ext == ".cube") {

			onDisk.insert(entry.path().stem().string());
		}
	}

	std::unordered_set<std::string> loaded;
	loaded.reserve(textures_.size());
	for (const auto& [name, _] : textures_) {

		loaded.insert(name);
	}

	std::vector<std::string> notLoaded;
	notLoaded.reserve(onDisk.size());
	for (auto& stem : onDisk) {
		if (!loaded.contains(stem)) {

			notLoaded.emplace_back(stem);
		}
	}

	if (notLoaded.empty()) {

		LOG_ASSET_INFO("[Texture] NotLoaded(on disk only): 0");
	} else {

		LOG_ASSET_INFO("[Texture] NotLoaded(on disk only): {}", notLoaded.size());
		if (listAll) {
			for (auto& n : notLoaded) {

				LOG_ASSET_INFO("  - {}", n);
			}
		}
	}
}

const D3D12_GPU_DESCRIPTOR_HANDLE& TextureManager::GetGPUHandle(const std::string textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {

		LOG_WARN("texture not found → {}", textureName);
		ASSERT(FALSE, "not found texture" + textureName);
	}

	// 使用された
	it->second.isUse = true;
	return it->second.gpuHandle;
}

uint32_t TextureManager::GetTextureGPUIndex(const std::string& textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {

		LOG_WARN("texture not found → {}", textureName);
		ASSERT(FALSE, "not found texture" + textureName);
	}

	// 使用された
	it->second.isUse = true;
	return it->second.srvIndex;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string textureName) const {

	auto it = textures_.find(textureName);
	if (it == textures_.end()) {

		LOG_WARN("texture not found → {}", textureName);
		ASSERT(FALSE, "not found texture" + textureName);
	}

	// 使用された
	it->second.isUse = true;
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

void TextureManager::RequestLoadAsync(const std::string& textureName) {

	// 既にロード済みなら何もしない
	{
		std::scoped_lock lk(gpuMutex_);
		if (textures_.contains(textureName)) return;
	}
	// 多重投入防止（簡易: キュー内重複チェック）
	{
		std::scoped_lock lk(jobMutex_);
		for (auto& j : jobs_) {
			if (j == textureName) return;
		}
		jobs_.push_back(textureName);
		SpdLogger::Log("[Texture][Enqueue] " + textureName);
	}
	jobCv_.notify_one();
}

void TextureManager::WaitAll() {

	// ジョブ枯渇待ち（簡易）
	for (;;) {
		{
			std::scoped_lock lk(jobMutex_);
			if (jobs_.empty()) {

				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

bool TextureManager::FindTexturePath(const std::string& textureName, std::filesystem::path& outPath) {

	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (entry.is_regular_file() && entry.path().stem().string() == textureName) {
			const std::string ext = entry.path().extension().string();
			if (ext == ".png" || ext == ".jpg" || ext == ".dds") {
				outPath = entry.path();
				return true;
			}
		}
	}
	return false;
}

DirectX::ScratchImage TextureManager::DecodeAndGenMips(
	const std::filesystem::path& filePath, DirectX::TexMetadata& outMeta) {

	using namespace DirectX;
	ScratchImage img{};
	const auto wpath = ConvertString(filePath.string());

	HRESULT hr = S_OK;
	if (wpath.ends_with(L".dds")) {
		hr = LoadFromDDSFile(wpath.c_str(), DDS_FLAGS_NONE, nullptr, img);
	} else {
		hr = LoadFromWICFile(wpath.c_str(), WIC_FLAGS_FORCE_SRGB | WIC_FLAGS_DEFAULT_SRGB, nullptr, img);
	}
	assert(SUCCEEDED(hr));

	ScratchImage mip;
	if (IsCompressed(img.GetMetadata().format)) {
		mip = std::move(img);
	} else {
		hr = DirectX::GenerateMipMaps(img.GetImages(), img.GetImageCount(), img.GetMetadata(),
			DirectX::TEX_FILTER_SRGB, 4, mip);
		assert(SUCCEEDED(hr));
	}
	outMeta = mip.GetMetadata();
	return mip;
}

void TextureManager::WorkerLoop() {

	while (!stop_) {
		std::string name;
		{
			std::unique_lock lk(jobMutex_);
			jobCv_.wait(lk, [&] { return stop_ || !jobs_.empty(); });
			if (stop_) break;
			name = std::move(jobs_.front());
			jobs_.pop_front();
		}

		// 重複ロード（他スレッドで完了済み）チェック
		{
			std::scoped_lock lk(gpuMutex_);
			if (textures_.contains(name)) continue;
		}

		SpdLogger::Log("[Texture][Begin] " + name);

		std::filesystem::path path;
		if (!FindTexturePath(name, path)) {
			// ログ・アサートはお好みで
			continue;
		}

		// CPU重い部分（WIC/DDSロード, MIP生成）
		auto t0 = std::chrono::high_resolution_clock::now();
		DirectX::TexMetadata meta{};
		auto mip = DecodeAndGenMips(path, meta);
		auto t1 = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
		SpdLogger::Log("[Texture][Decoded] " + name + "  " +
			std::to_string(meta.width) + "x" + std::to_string(meta.height) +
			" mips=" + std::to_string(meta.mipLevels) +
			" (" + std::to_string(ms) + "ms)");

		// 階層などのメタはファイル名から計算
		const std::string identifier = path.stem().string();

		// GPU転送
		CreateAndUpload(identifier, mip, meta);

		// 階層（相対パス）を後から設定
		{
			std::scoped_lock lk(gpuMutex_);
			TextureData& t = textures_[identifier];

			std::filesystem::path relative = std::filesystem::relative(path, baseDirectoryPath_);
			relative.replace_extension();
			t.hierarchy = relative.generic_string();
		}
	}
}

void TextureManager::CreateAndUpload(const std::string& identifier,
	const DirectX::ScratchImage& mipImages, const DirectX::TexMetadata& meta) {

	// GPUリソース作成
	ComPtr<ID3D12Resource> tex;
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Width = UINT(meta.width);
		desc.Height = UINT(meta.height);
		desc.MipLevels = UINT16(meta.mipLevels);
		desc.DepthOrArraySize = UINT16(meta.arraySize);
		desc.Format = meta.format;
		desc.SampleDesc.Count = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION(meta.dimension);

		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_DEFAULT;

		HRESULT hr = device_->CreateCommittedResource(
			&heap, D3D12_HEAP_FLAG_NONE, &desc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&tex));
		assert(SUCCEEDED(hr));
	}

	// サブリソース
	std::vector<D3D12_SUBRESOURCE_DATA> subs;
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), meta, subs);

	// 中間バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuf;
	{
		uint64_t size = GetRequiredIntermediateSize(tex.Get(), 0, (UINT)subs.size());

		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC buf{};
		buf.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buf.Width = size; buf.Height = 1; buf.DepthOrArraySize = 1;
		buf.MipLevels = 1; buf.SampleDesc.Count = 1; buf.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		HRESULT hr = device_->CreateCommittedResource(
			&heap, D3D12_HEAP_FLAG_NONE, &buf, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuf));
		assert(SUCCEEDED(hr));
	}

	auto* cl = dxUploadCommand_->GetCommandList();
	UpdateSubresources(cl, tex.Get(), uploadBuf.Get(), 0, 0, (UINT)subs.size(), subs.data());

	// バリア: COPY_DEST -> GENERIC_READ
	D3D12_RESOURCE_BARRIER b{};
	b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	b.Transition.pResource = tex.Get();
	b.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	b.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cl->ResourceBarrier(1, &b);

	// 実行＆待機（描画キューを止めない）
	dxUploadCommand_->ExecuteCommands();
	SpdLogger::Log(std::string("[Texture][Upload->GPU][End]  ") + identifier);

	// SRV生成と登録（競合防止のためロック）
	std::scoped_lock lk(gpuMutex_);

	TextureData& t = textures_[identifier];
	t.resource = tex;
	t.metadata = meta;
	t.isUse = false;

	// SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
	srv.Format = meta.format;
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (meta.IsCubemap()) {
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srv.TextureCube.MostDetailedMip = 0;
		srv.TextureCube.MipLevels = UINT_MAX;
		srv.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels = (UINT)meta.mipLevels;
	}

	srvDescriptor_->CreateSRV(t.srvIndex, t.resource.Get(), srv);
	t.gpuHandle = srvDescriptor_->GetGPUHandle(t.srvIndex);

	std::wstring wname(identifier.begin(), identifier.end());
	t.resource->SetName(wname.c_str());

	isCacheValid_ = false;
}