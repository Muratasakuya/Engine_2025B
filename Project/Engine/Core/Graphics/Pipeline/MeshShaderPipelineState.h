#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComPtr.h>

// directX
#include <d3d12.h>
#include <dxcapi.h>
#include <Externals/DirectX12/d3dx12.h>

//============================================================================
//	MeshShaderPipelineState class
//============================================================================
class MeshShaderPipelineState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshShaderPipelineState() = default;
	~MeshShaderPipelineState() = default;

	void Create(ID3D12Device8* device, class DxShaderCompiler* shaderCompiler,
		class SRVManager* srvManager);

	//--------- accessor -----------------------------------------------------

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
	ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ComPtr<ID3D12PipelineState> pipelineState_;

	// shaderBlob
	ComPtr<IDxcBlob> msShaderBlob_;
	ComPtr<IDxcBlob> psShaderBlob_;

	// RootSignature
	ComPtr<ID3D12RootSignature> rootSignature_;
	// rasterize設定
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	// RTのblend設定
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendStateDesc_{};
	// blendState設定
	D3D12_BLEND_DESC blendDesc_{};
	// depthStencilop設定
	D3D12_DEPTH_STENCILOP_DESC depthStencilopDesc_{};
	// depthStencil設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	// shaderByteCode設定
	D3D12_SHADER_BYTECODE msShaderByteCode_{};
	D3D12_SHADER_BYTECODE psShaderByteCode_{};
	// sampleDesc設定
	DXGI_SAMPLE_DESC sampleDesc_{};
	// RTのformat設定
	D3D12_RT_FORMAT_ARRAY renderTargetFoamat_{};
};