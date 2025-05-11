#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/DxUtils.h>

// directX
#include <d3d12.h>
// c++
#include <vector>
#include <cstdint>
#include <optional>
#include <cassert>

//============================================================================
//	DxConstBuffer class
//============================================================================
template<typename T>
class DxConstBuffer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DxConstBuffer() = default;
	virtual ~DxConstBuffer() = default;

	// 初期化
	void CreateConstBuffer(ID3D12Device* device);
	void CreateStructuredBuffer(ID3D12Device* device, UINT instanceCount);
	void CreateUavStructuredBuffer(ID3D12Device* device, UINT instanceCount);

	void CreateVertexBuffer(ID3D12Device* device, UINT vertexCount);
	void CreateUavVertexBuffer(ID3D12Device* device, UINT vertexCount);
	void CreateIndexBuffer(ID3D12Device* device, UINT indexCount);

	// 転送
	void TransferData(const T& data);
	void TransferVectorData(const std::vector<T>& data);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetResource() const;

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBuffer() const;
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBuffer() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12Resource> resource_;
	T* mappedData_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	//--------- functions ----------------------------------------------------

	ComPtr<ID3D12Resource> CreateUavVertexResource(ID3D12Device* device, size_t sizeInBytes);
};

//============================================================================
//	DxConstBuffer templateMethods
//============================================================================

template<typename T>
inline void DxConstBuffer<T>::CreateConstBuffer(ID3D12Device* device) {

	DxUtils::CreateBufferResource(device, resource_, sizeof(T));

	// マッピング
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	assert(SUCCEEDED(hr));
}

template<typename T>
inline void DxConstBuffer<T>::CreateStructuredBuffer(ID3D12Device* device, UINT instanceCount) {

	DxUtils::CreateBufferResource(device, resource_, sizeof(T) * instanceCount);

	// マッピング
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	assert(SUCCEEDED(hr));
}

template<typename T>
inline void DxConstBuffer<T>::CreateUavStructuredBuffer(ID3D12Device* device, UINT instanceCount) {

	DxUtils::CreateUavBufferResource(device, resource_, sizeof(T) * instanceCount);

	// マッピング
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	assert(SUCCEEDED(hr));
}

template<typename T>
inline void DxConstBuffer<T>::CreateVertexBuffer(ID3D12Device* device, UINT vertexCount) {

	HRESULT hr;

	if (vertexCount > 0) {

		// 頂点データサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(T) * vertexCount);

		// 定数バッファーのリソース作成
		DxUtils::CreateBufferResource(device, resource_, sizeVB);

		// 頂点バッファビューの作成
		vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = sizeVB;
		vertexBufferView_.StrideInBytes = sizeof(T);

		// マッピング
		hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
		assert(SUCCEEDED(hr));
	}
}

template<typename T>
inline void DxConstBuffer<T>::CreateUavVertexBuffer(ID3D12Device* device, UINT vertexCount) {

	if (vertexCount > 0) {

		// 頂点データサイズ
		UINT sizeVB = static_cast<UINT>(sizeof(T) * vertexCount);

		// 定数バッファーのリソース作成
		resource_ = CreateUavVertexResource(device, sizeVB);

		// 頂点バッファビューの作成
		vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = sizeVB;
		vertexBufferView_.StrideInBytes = sizeof(T);
	}
}

template<typename T>
inline void DxConstBuffer<T>::CreateIndexBuffer(ID3D12Device* device, UINT indexCount) {

	HRESULT hr;

	if (indexCount > 0) {

		// インデックスデータのサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(T) * indexCount);

		// 定数バッファーのリソース作成
		DxUtils::CreateBufferResource(device, resource_, sizeIB);

		indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView_.SizeInBytes = sizeIB;

		// マッピング
		hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
		assert(SUCCEEDED(hr));
	}
}

template<typename T>
inline void DxConstBuffer<T>::TransferData(const T& data) {

	if (mappedData_) {

		std::memcpy(mappedData_, &data, sizeof(T));
	}
}

template<typename T>
inline void DxConstBuffer<T>::TransferVectorData(const std::vector<T>& data) {

	if (mappedData_) {

		std::memcpy(mappedData_, data.data(), sizeof(T) * data.size());
	}
}

template<typename T>
inline ID3D12Resource* DxConstBuffer<T>::GetResource() const {
	return resource_.Get();
}

template<typename T>
inline const D3D12_VERTEX_BUFFER_VIEW& DxConstBuffer<T>::GetVertexBuffer() const {
	return vertexBufferView_;
}

template<typename T>
inline const D3D12_INDEX_BUFFER_VIEW& DxConstBuffer<T>::GetIndexBuffer() const {
	return indexBufferView_;
}

template<typename T>
inline ComPtr<ID3D12Resource> DxConstBuffer<T>::CreateUavVertexResource(ID3D12Device* device, size_t sizeInBytes) {

	HRESULT hr;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// リソースのサイズ
	vertexResourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	// 実際に頂点リソースを作る
	ComPtr<ID3D12Resource> bufferResource = nullptr;
	hr = device->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(hr));

	return bufferResource;
}