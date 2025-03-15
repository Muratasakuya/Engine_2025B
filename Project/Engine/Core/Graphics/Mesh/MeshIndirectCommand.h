#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Engine/Core/Lib/ComPtr.h>

// directX
#include <d3d12.h>
// c++
#include <string>
#include <unordered_map>
#include <cstdint>
#include <ranges>
// front
class DxCommand;

// 方針、instancing描画と同じようにIA，textureが一緒のものをまとめて描画を行う
// bufferをComponentManagerから通知されたら作成する

//============================================================================
//	MeshIndirectCommand class
//============================================================================
class MeshIndirectCommand {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshIndirectCommand() = default;
	~MeshIndirectCommand() = default;

	void Init(ID3D12Device* device, ID3D12RootSignature* rootSignature);

	void Create(const std::string& name, uint32_t objectCount);

	void Execute(DxCommand* dxCommand);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 最大material数
	constexpr static const uint32_t kMaxMaterialCount_ = 4;

	struct IndirectCommand {

		// bufferAdress
		D3D12_GPU_VIRTUAL_ADDRESS matrixBufferAddress;
		D3D12_GPU_VIRTUAL_ADDRESS materialBufferAddress[kMaxMaterialCount_];

		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;

	ComPtr<ID3D12CommandSignature> commandSignature_;

	std::unordered_map<std::string, ComPtr<ID3D12Resource>> indirectBuffers_;
	std::unordered_map<std::string, UINT> objectCounts_;

	//--------- functions ----------------------------------------------------

	void CreateCommandSignature(ID3D12Device* device, ID3D12RootSignature* rootSignature);

	void CreateIndirectCommandBuffer(ID3D12Device* device, const std::string& name, uint32_t objectCount);
};