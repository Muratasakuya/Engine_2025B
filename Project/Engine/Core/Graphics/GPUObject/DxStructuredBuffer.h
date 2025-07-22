#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Lib/DxUtils.h>

// c++
#include <vector>

//============================================================================
//	DxStructuredBuffer class
//============================================================================
template<typename T>
class DxStructuredBuffer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DxStructuredBuffer() = default;
	virtual ~DxStructuredBuffer() = default;

	void CreateSRVBuffer(ID3D12Device* device, UINT instanceCount);
	void CreateUAVBuffer(ID3D12Device* device, UINT instanceCount);

	void TransferData(const std::vector<T>& data);
	void TransferData(const std::vector<T>& data, size_t count);

	//--------- accessor -----------------------------------------------------

	void SetSRVGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { srvGPUHandle_ = handle; }
	void SetUAVGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { uavGPUHandle_ = handle; }

	ID3D12Resource* GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVGPUHandle() const { return srvGPUHandle_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetUAVGPUHandle() const { return uavGPUHandle_; }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc(UINT instanceCount) const;
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(UINT instanceCount) const;

	bool IsCreatedResource() const { return isCreated_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12Resource> resource_;
	T* mappedData_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE uavGPUHandle_;

	bool isCreated_ = false;
};

//============================================================================
//	DxStructuredBuffer templateMethods
//============================================================================

template<typename T>
inline void DxStructuredBuffer<T>::CreateSRVBuffer(ID3D12Device* device, UINT instanceCount) {

	DxUtils::CreateBufferResource(device, resource_, sizeof(T) * instanceCount);

	// マッピング
	HRESULT hr = resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
	assert(SUCCEEDED(hr));

	isCreated_ = true;
}

template<typename T>
inline void DxStructuredBuffer<T>::CreateUAVBuffer(ID3D12Device* device, UINT instanceCount) {

	DxUtils::CreateUavBufferResource(device, resource_, sizeof(T) * instanceCount);
	// マッピング処理は行わない

	isCreated_ = true;
}

template<typename T>
inline void DxStructuredBuffer<T>::TransferData(const std::vector<T>& data) {

	if (mappedData_) {

		std::memcpy(mappedData_, data.data(), sizeof(T) * data.size());
	}
}

template<typename T>
inline void DxStructuredBuffer<T>::TransferData(const std::vector<T>& data, size_t count) {

	if (mappedData_) {

		std::memcpy(mappedData_, data.data(), sizeof(T) * count);
	}
}

template<typename T>
inline D3D12_SHADER_RESOURCE_VIEW_DESC DxStructuredBuffer<T>::GetSRVDesc(UINT instanceCount) const {

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = instanceCount;
	srvDesc.Buffer.StructureByteStride = sizeof(T);
	srvDesc.Buffer.FirstElement = 0;

	return srvDesc;
}

template<typename T>
inline D3D12_UNORDERED_ACCESS_VIEW_DESC DxStructuredBuffer<T>::GetUAVDesc(UINT instanceCount) const {

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.NumElements = instanceCount;
	uavDesc.Buffer.StructureByteStride = sizeof(T);
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.FirstElement = 0;

	return uavDesc;
}