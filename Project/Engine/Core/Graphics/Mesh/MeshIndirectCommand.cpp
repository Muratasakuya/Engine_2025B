#include "MeshIndirectCommand.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MeshIndirectCommand classMethods
//============================================================================

void MeshIndirectCommand::Init(ID3D12Device* device, ID3D12RootSignature* rootSignature) {

	device_ = nullptr;
	device_ = device;

	// commandSignatureの作成
	CreateCommandSignature(device, rootSignature);
}

void MeshIndirectCommand::Create(const std::string& name, uint32_t objectCount) {

	// すでにある場合は作成しない
	if (Algorithm::Find(indirectBuffers_, name)) {
		return;
	}

	// buffer作成
	CreateIndirectCommandBuffer(device_, name, objectCount);
}

void MeshIndirectCommand::Execute(DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	for (const auto& [name, indirectBuffer] : indirectBuffers_) {

		dxCommand->TransitionBarriers({ indirectBuffer.Get() },
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);

		// 描画処理実行
		commandList->ExecuteIndirect(
			commandSignature_.Get(), objectCounts_[name], indirectBuffer.Get(),
			0, nullptr, 0);

		dxCommand->TransitionBarriers({ indirectBuffer.Get() },
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
			D3D12_RESOURCE_STATE_GENERIC_READ);
	}
}

void MeshIndirectCommand::CreateCommandSignature(ID3D12Device* device, ID3D12RootSignature* rootSignature) {

	D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[3]{};

	// transform
	argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	argumentDescs[0].ConstantBufferView.RootParameterIndex = 2;

	// material
	argumentDescs[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	argumentDescs[1].ConstantBufferView.RootParameterIndex = 5;

	// index描画設定
	argumentDescs[2].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc{};
	commandSignatureDesc.ByteStride = sizeof(IndirectCommand);
	commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
	commandSignatureDesc.pArgumentDescs = argumentDescs;
	commandSignatureDesc.NodeMask = 0;

	HRESULT hr = device->CreateCommandSignature(
		&commandSignatureDesc, rootSignature, IID_PPV_ARGS(&commandSignature_));
	if (FAILED(hr)) {
		ASSERT(FALSE, "failed CreateCommandSignature");
	}
}

void MeshIndirectCommand::CreateIndirectCommandBuffer(
	ID3D12Device* device, const std::string& name, uint32_t objectCount) {

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// IndirectCommand * objectの最大数
	bufferDesc.Width = sizeof(IndirectCommand) * objectCount;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	indirectBuffers_[name] = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indirectBuffers_[name]));

	if (FAILED(hr)) {
		ASSERT(FALSE, "failed CreateCommittedResource");
	}
}