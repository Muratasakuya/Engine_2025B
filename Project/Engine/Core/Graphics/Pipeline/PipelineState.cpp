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
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	PipelineState classMethods
//============================================================================

void PipelineState::Create(const std::string& fileName, ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// pipelineの種類
	PipelineType pipelineType{};

	// jsonFileの読み込み
	Json json = LoadFile(fileName);

	// shaderCompileを行う
	std::vector<ComPtr<IDxcBlob>> shaderBlobs;
	shaderCompiler->Compile(json, shaderBlobs);
	// shaderBlobsのサイズが1つならcomputeShaderとして処理をする
	if (shaderBlobs.size() == 1) {

		pipelineType = PipelineType::COMPUTE;
	} else {
		// meshShaderを使うかどうか
		if (json.contains("Type") && json["Type"] == "MS") {

			// typeがMS
			pipelineType = PipelineType::MESH;
		} else {

			// typeがMS
			pipelineType = PipelineType::VERTEX;
		}
	}

	// rootSignatureの作成
	DxRootSignature dxRootSignature;
	dxRootSignature.Create(fileName, json, device, srvDescriptor, rootSignature_);

	// pipelineごとの分岐処理
	// pipeline作成
	switch (pipelineType) {
	case PipelineState::PipelineType::VERTEX: {

		CreateVertexPipeline(fileName, json, device, shaderBlobs);
		break;
	}
	case PipelineState::PipelineType::MESH: {

		CreateMeshPipeline(fileName, json, device, shaderBlobs);
		break;
	}
	case PipelineState::PipelineType::COMPUTE: {

		CreateComputePipeline(fileName, device, shaderBlobs);
		break;
	}
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

void PipelineState::CreateVertexPipeline(const std::string& fileName, const Json& json, ID3D12Device8* device,
	const std::vector<ComPtr<IDxcBlob>>& shaderBlobs) {

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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	// 共通設定
	pipelineDesc.pRootSignature = rootSignature_.Get();
	pipelineDesc.VS = {
		.pShaderBytecode = shaderBlobs.front()->GetBufferPointer(),
		.BytecodeLength = shaderBlobs.front()->GetBufferSize()
	};
	pipelineDesc.PS = {
		.pShaderBytecode = shaderBlobs.back()->GetBufferPointer(),
		.BytecodeLength = shaderBlobs.back()->GetBufferSize()
	};
	pipelineDesc.RasterizerState = rasterizerDesc;
	pipelineDesc.DepthStencilState = depthDesc;
	pipelineDesc.SampleDesc.Count = 1;
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	pipelineDesc.NumRenderTargets = 1;

	// inputLayoutがあれば設定する
	if (inputDesc.has_value()) {

		pipelineDesc.InputLayout = *inputDesc;
	} else {

		pipelineDesc.InputLayout.pInputElementDescs = nullptr;
		pipelineDesc.InputLayout.NumElements = 0;
	}

	const auto& stateJson = json["PipelineState"][0];
	std::string topologyType = stateJson.value("TopologyType", "TRIANGLE");
	std::string dsvFormat = stateJson["DSVFormat"];
	std::string rtvFormat = stateJson["RTVFormats"];
	std::string blendMode = stateJson["BlendMode"];

	if (topologyType == "TRIANGLE") {

		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	} else if (topologyType == "LINE") {

		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}

	if (dsvFormat == "D24_UNORM_S8_UINT") {

		pipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	} else if (dsvFormat == "D32_FLOAT") {

		pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	}

	if (rtvFormat == "R32G32B32A32_FLOAT") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	} else if (rtvFormat == "R8G8B8A8_UNORM_SRGB") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	} else if (rtvFormat == "R32_FLOAT") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	}

	// 全てのblendModeで作成
	if (blendMode == "ALL") {
		for (const auto& blend : Algorithm::GetEnumArray(BlendMode::BlendCount)) {

			// BlendState
			DxBlendState dxBlendState;
			D3D12_RENDER_TARGET_BLEND_DESC blendState;
			dxBlendState.Create(static_cast<BlendMode>(blend), blendState);
			pipelineDesc.BlendState.RenderTarget[0] = blendState;

			// 生成
			graphicsPipelinepipelineStates_[blend] = nullptr;
			HRESULT hr = device->CreateGraphicsPipelineState(
				&pipelineDesc,
				IID_PPV_ARGS(&graphicsPipelinepipelineStates_[blend]));
			if (FAILED(hr)) {

				const std::string& file = "FileName: " + fileName + "\n";
				ASSERT(FALSE, file + "Filed create Pipeline");
			}
		}
	}
	// それ以外はすべてnormalで作成
	else {

		// BlendState
		DxBlendState dxBlendState;
		D3D12_RENDER_TARGET_BLEND_DESC blendState;
		dxBlendState.Create(static_cast<BlendMode>(kBlendModeNormal), blendState);
		pipelineDesc.BlendState.RenderTarget[0] = blendState;

		// 生成
		graphicsPipelinepipelineStates_[kBlendModeNormal] = nullptr;
		HRESULT hr = device->CreateGraphicsPipelineState(
			&pipelineDesc,
			IID_PPV_ARGS(&graphicsPipelinepipelineStates_[kBlendModeNormal]));
		if (FAILED(hr)) {

			const std::string& file = "FileName: " + fileName + "\n";
			ASSERT(FALSE, file + "Filed create Pipeline");
		}
	}
}


void PipelineState::CreateMeshPipeline(const std::string& fileName, const Json& json, ID3D12Device8* device,
	const std::vector<ComPtr<IDxcBlob>>& shaderBlobs) {

	// depth、rasterizerの作成
	DxDepthRaster depthRaster;
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	D3D12_DEPTH_STENCIL_DESC depthDesc{};
	depthRaster.Create(json, rasterizerDesc, depthDesc);

	// sampleDesc設定
	DXGI_SAMPLE_DESC sampleDesc{};
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipelineDesc{};

	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.SampleMask = UINT_MAX;

	const auto& stateJson = json["PipelineState"][0];
	std::string dsvFormat = stateJson["DSVFormat"];
	std::string rtvFormat = stateJson["RTVFormats"];
	std::string blendMode = stateJson["BlendMode"];
	if (dsvFormat == "D24_UNORM_S8_UINT") {

		pipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	} else if (dsvFormat == "D32_FLOAT") {

		pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	}

	if (rtvFormat == "R32G32B32A32_FLOAT") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	} else if (rtvFormat == "R8G8B8A8_UNORM_SRGB") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	} else if (rtvFormat == "R32_FLOAT") {

		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
	}

	pipelineDesc.pRootSignature = rootSignature_.Get();

	// shaderByteCodeの設定
	pipelineDesc.MS = {
		shaderBlobs.front().Get()->GetBufferPointer(),
		shaderBlobs.front().Get()->GetBufferSize()
	};
	pipelineDesc.PS = {
		shaderBlobs.back().Get()->GetBufferPointer(),
		shaderBlobs.back().Get()->GetBufferSize()
	};

	// 全てのblendModeで作成
	if (blendMode == "ALL") {
		for (const auto& blend : Algorithm::GetEnumArray(BlendMode::BlendCount)) {

			// BlendState
			DxBlendState dxBlendState;
			D3D12_RENDER_TARGET_BLEND_DESC blendState;
			dxBlendState.Create(static_cast<BlendMode>(blend), blendState);
			pipelineDesc.BlendState.RenderTarget[0] = blendState;

			pipelineDesc.RasterizerState = rasterizerDesc;
			pipelineDesc.BlendState.RenderTarget[0] = blendState;
			pipelineDesc.DepthStencilState = depthDesc;
			pipelineDesc.SampleDesc = sampleDesc;

			auto pipelineStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipelineDesc);

			D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
			streamDesc.pPipelineStateSubobjectStream = &pipelineStream;
			streamDesc.SizeInBytes = sizeof(pipelineStream);

			// 生成
			HRESULT hr = device->CreatePipelineState(&streamDesc,
				IID_PPV_ARGS(graphicsPipelinepipelineStates_[blend].GetAddressOf()));
			if (FAILED(hr)) {

				const std::string& file = "FileName: " + fileName + "\n";
				ASSERT(FALSE, file + "Filed create Pipeline");
			}
		}
	}
	// それ以外はすべてnormalで作成
	else {

		// BlendState
		DxBlendState dxBlendState;
		D3D12_RENDER_TARGET_BLEND_DESC blendState;
		dxBlendState.Create(static_cast<BlendMode>(kBlendModeNormal), blendState);
		pipelineDesc.BlendState.RenderTarget[0] = blendState;

		pipelineDesc.RasterizerState = rasterizerDesc;
		pipelineDesc.BlendState.RenderTarget[0] = blendState;
		pipelineDesc.DepthStencilState = depthDesc;
		pipelineDesc.SampleDesc = sampleDesc;

		auto pipelineStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipelineDesc);

		D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		streamDesc.pPipelineStateSubobjectStream = &pipelineStream;
		streamDesc.SizeInBytes = sizeof(pipelineStream);

		// 生成
		HRESULT hr = device->CreatePipelineState(&streamDesc,
			IID_PPV_ARGS(graphicsPipelinepipelineStates_[kBlendModeNormal].GetAddressOf()));
		if (FAILED(hr)) {

			const std::string& file = "FileName: " + fileName + "\n";
			ASSERT(FALSE, file + "Filed create Pipeline");
		}
	}
}

void PipelineState::CreateComputePipeline(const std::string& fileName, ID3D12Device8* device,
	const std::vector<ComPtr<IDxcBlob>>& shaderBlobs) {

	// pipelinsStateの作成
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineDesc{};

	pipelineDesc.CS = {
	.pShaderBytecode = shaderBlobs.back()->GetBufferPointer(),
	.BytecodeLength = shaderBlobs.back()->GetBufferSize()
	};
	pipelineDesc.pRootSignature = rootSignature_.Get();

	// 生成
	HRESULT hr = device->CreateComputePipelineState(
		&pipelineDesc,
		IID_PPV_ARGS(&computePipelineState_));
	if (FAILED(hr)) {

		const std::string& file = "FileName: " + fileName + "\n";
		ASSERT(FALSE, file + "Filed create Pipeline");
	}
}