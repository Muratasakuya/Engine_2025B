#include "PipelineState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Asset/Filesystem.h>
#include <Engine/Core/Graphics/Pipeline/DxRootSignature.h>
#include <Engine/Core/Graphics/Pipeline/DxInputLayout.h>
#include <Engine/Core/Graphics/Pipeline/DxDepthRaster.h>
#include <Engine/Core/Graphics/Pipeline/DxBlendState.h>

//============================================================================
//	PipelineState classMethods
//============================================================================

void PipelineState::Create(const std::string& fileName, ID3D12Device* device,
	DxShaderCompiler* shaderCompiler) {

	// jsonFileの読み込み
	Json json = LoadFile(fileName);

	// shaderCompile
	std::vector<ComPtr<IDxcBlob>> shaderBlobs;
	shaderCompiler->Compile(json, shaderBlobs);
	bool isCompute = shaderBlobs.size() == 1;

	// rootSignatureの作成
	DxRootSignature dxRootSignature;
	dxRootSignature.Create(json, device, rootSignature_);

	// ComputeShaderの時以外行う
	if (!isCompute) {

		// inputLayoutの作成
		DxInputLayout dxInputLayout;
		std::optional<D3D12_INPUT_LAYOUT_DESC> inputDesc;
		dxInputLayout.Create(json, inputDesc);

		// depth、rasterizerの作成
		DxDepthRaster depthRaster;
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		D3D12_DEPTH_STENCIL_DESC depthDesc{};
		depthRaster.Create(json, rasterizerDesc, depthDesc);

		// pipelinsStateの作成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};

		// 共通設定
		pipelineStateDesc.pRootSignature = rootSignature_.Get();
		pipelineStateDesc.VS = {
			.pShaderBytecode = shaderBlobs.front()->GetBufferPointer(),
			.BytecodeLength = shaderBlobs.front()->GetBufferSize()
		};
		pipelineStateDesc.PS = {
			.pShaderBytecode = shaderBlobs.back()->GetBufferPointer(),
			.BytecodeLength = shaderBlobs.back()->GetBufferSize()
		};
		pipelineStateDesc.RasterizerState = rasterizerDesc;
		pipelineStateDesc.DepthStencilState = depthDesc;
		pipelineStateDesc.SampleDesc.Count = 1;
		pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		pipelineStateDesc.NumRenderTargets = 1;

		if (inputDesc.has_value()) {
			pipelineStateDesc.InputLayout = *inputDesc;
		} else {
			pipelineStateDesc.InputLayout.pInputElementDescs = nullptr;
			pipelineStateDesc.InputLayout.NumElements = 0;
		}

		bool blendAll = false;
		// BlendModeがALLならすべてのBlendModeで作成し、NONEならNormalでのみ作成する
		if (json.contains("PipelineState") && json["PipelineState"].is_array() && !json["PipelineState"].empty()) {

			const auto& stateJson = json["PipelineState"][0];
			blendAll = (stateJson.value("BlendMode", "ALL") == "ALL") ? TRUE : FALSE;

			std::string topologyType= stateJson.value("TopologyType", "TRIANGLE");
			std::string dsvFormat = stateJson.value("DSVFormat", "D24_UNORM_S8_UINT");
			std::string rtvFormat = stateJson.value("RTVFormats", "R32G32B32A32_FLOAT");

			if (topologyType == "TRIANGLE") {
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			}else if (topologyType == "LINE") {
				pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			}

			if (dsvFormat == "D24_UNORM_S8_UINT") {
				pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			} else if (dsvFormat == "D32_FLOAT") {
				pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			}

			if (rtvFormat == "R32G32B32A32_FLOAT") {
				pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			} else if (rtvFormat == "R8G8B8A8_UNORM_SRGB") {
				pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			} else if (rtvFormat == "R32_FLOAT") {
				pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
			}
		}

		// すべてのBlendModeで作成
		if (blendAll) {
			graphicsPipelinepipelineStates_.resize(blendModeNum);
			for (const auto& blendMode : blendModeTypes) {

				// BlendState
				DxBlendState dxBlendState;
				D3D12_RENDER_TARGET_BLEND_DESC blendState;
				dxBlendState.Create(blendMode, blendState);
				pipelineStateDesc.BlendState.RenderTarget[0] = blendState;

				// 生成
				HRESULT hr = device->CreateGraphicsPipelineState(
					&pipelineStateDesc,
					IID_PPV_ARGS(&graphicsPipelinepipelineStates_[blendMode]));
				assert(SUCCEEDED(hr));
			}
		} else {

			graphicsPipelinepipelineStates_.resize(1);
			// BlendState
			DxBlendState dxBlendState;
			D3D12_RENDER_TARGET_BLEND_DESC blendState;
			dxBlendState.Create(kBlendModeNormal, blendState);
			pipelineStateDesc.BlendState.RenderTarget[0] = blendState;

			// 生成
			graphicsPipelinepipelineStates_.front() = nullptr;
			HRESULT hr = device->CreateGraphicsPipelineState(
				&pipelineStateDesc,
				IID_PPV_ARGS(&graphicsPipelinepipelineStates_.front()));
			assert(SUCCEEDED(hr));
		}
	} else {

		// pipelinsStateの作成
		D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc{};

		pipelineStateDesc.CS = {
		.pShaderBytecode = shaderBlobs.back()->GetBufferPointer(),
		.BytecodeLength = shaderBlobs.back()->GetBufferSize()
		};
		pipelineStateDesc.pRootSignature = rootSignature_.Get();

		// 生成
		HRESULT hr = device->CreateComputePipelineState(
			&pipelineStateDesc,
			IID_PPV_ARGS(&computePipelineState_));
		assert(SUCCEEDED(hr));
	}
}

ID3D12PipelineState* PipelineState::GetGraphicsPipeline(BlendMode blendMode) const {
	return graphicsPipelinepipelineStates_[blendMode].Get();
}

Json PipelineState::LoadFile(const std::string& fileName) {

	const fs::path basePath = "./Assets/Engine/ShaderData/";
	fs::path fullPath;

	if (!Filesystem::Found(basePath, fileName, fullPath)) {
		// ファイルが見つからなかった場合
		ASSERT(false, "Failed to find file: " + fileName);
	}

	std::ifstream file(fullPath);
	if (!file.is_open()) {
		ASSERT(false, "Failed to open file: " + fullPath.string());
	}

	Json json;
	file >> json;

	return json;
}