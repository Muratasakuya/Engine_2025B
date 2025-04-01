#include "MSInputAssembler.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	MSInputAssembler classMethods
//============================================================================

namespace {

	struct MSVertexData {

		Vector4 pos;
		Color color;
	};
}// namespace

void MSInputAssembler::Create(ID3D12Device* device, SRVManager* srvManager) {

	// 頂点データ作成
	CreateVertexBuffer(device, srvManager);
	CreateIndexBuffer(device, srvManager);
}

void MSInputAssembler::CreateVertexBuffer(
	ID3D12Device* device, SRVManager* srvManager) {

	// 頂点データ
	MSVertexData vertices[] = {

		{.pos = Vector4(-1.0f,-1.0f,0.0f,1.0f),.color = Color::Blue()},
		{.pos = Vector4(1.0f,-1.0f,0.0f,1.0f),.color = Color::Green()},
		{.pos = Vector4(0.0f,1.0f,0.0f,1.0f),.color = Color::Red()},
	};

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(vertices);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを作成
	HRESULT hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertexResource_.GetAddressOf()));
	if (FAILED(hr)) {
		return;
	}

	// マッピング処理
	void* ptr = nullptr;
	hr = vertexResource_->Map(0, nullptr, &ptr);
	if (FAILED(hr)) {
		return;
	}

	// 頂点データをマッピング先に設定
	std::memcpy(ptr, vertices, sizeof(vertices));

	// マッピング解除
	vertexResource_->Unmap(0, nullptr);

	// desc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.Buffer.FirstElement = 0;
	viewDesc.Buffer.NumElements = 3;
	viewDesc.Buffer.StructureByteStride = sizeof(MSVertexData);
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// SRV作成
	uint32_t srvIndex = 0;
	srvManager->CreateSRV(srvIndex, vertexResource_.Get(), viewDesc);
	// GPUHandle取得
	vertexGPUHandle_ = srvManager->GetGPUHandle(srvIndex);
}

void MSInputAssembler::CreateIndexBuffer(
	ID3D12Device* device, SRVManager* srvManager) {

	// インデックスデータ
	uint32_t indices[] = { 0,1,2 };

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = sizeof(indices);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを作成
	HRESULT hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(indexResource_.GetAddressOf()));
	if (FAILED(hr)) {
		return;
	}

	// マッピング処理
	void* ptr = nullptr;
	hr = indexResource_->Map(0, nullptr, &ptr);
	if (FAILED(hr)) {
		return;
	}

	// 頂点データをマッピング先に設定
	std::memcpy(ptr, indices, sizeof(indices));

	// マッピング解除
	indexResource_->Unmap(0, nullptr);

	// desc設定
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	viewDesc.Format = DXGI_FORMAT_UNKNOWN;
	viewDesc.Buffer.FirstElement = 0;
	viewDesc.Buffer.NumElements = 3;
	viewDesc.Buffer.StructureByteStride = sizeof(uint32_t);
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// SRV作成
	uint32_t srvIndex = 0;
	srvManager->CreateSRV(srvIndex, indexResource_.Get(), viewDesc);
	// GPUHandle取得
	indexGPUHandle_ = srvManager->GetGPUHandle(srvIndex);
}