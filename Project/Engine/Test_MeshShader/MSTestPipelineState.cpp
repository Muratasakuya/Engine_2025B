#include "MSTestPipelineState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>

//============================================================================
//	MSTestPipelineState classMethods
//============================================================================

void MSTestPipelineState::Create(ID3D12Device8* device, DxShaderCompiler* shaderCompiler) {

	// shaderModelのチェック
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
	auto hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
	if (FAILED(hr) || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5)) {

		ASSERT(FALSE, "shaderModel 6.5 is not supported");
	}

	// meshShaderに対応しているかチェック
	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));
	if (FAILED(hr) || (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)) {

		ASSERT(FALSE, "meshShaders aren't supported");
	}

	// shaderCompile
	{
		shaderCompiler->CompileShader(
			L"./Assets/Engine/Shaders/MS/Triangle.MS.hlsl", L"ms_6_5", msShaderBlob_);
		shaderCompiler->CompileShader(
			L"./Assets/Engine/Shaders/MS/Triangle.PS.hlsl", L"ps_6_0", psShaderBlob_);
	}

	// rootSignature作成
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		D3D12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[0].Descriptor.ShaderRegister = 0;
		rootParameters[0].Descriptor.RegisterSpace = 0;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;

		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[1].Descriptor.ShaderRegister = 1;
		rootParameters[1].Descriptor.RegisterSpace = 0;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;

		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[2].Descriptor.ShaderRegister = 0;
		rootParameters[2].Descriptor.RegisterSpace = 0;
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = flag;

		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		// バイナリをもとに生成
		hr = D3D12SerializeRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {

			const char* errorMessage = reinterpret_cast<const char*>(errorBlob->GetBufferPointer());
			ASSERT(false, errorMessage);
		}
		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
		assert(SUCCEEDED(hr));
	}

	// rasterizer
	{
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
		rasterizerDesc_.FrontCounterClockwise = FALSE;
		rasterizerDesc_.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerDesc_.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc_.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesc_.DepthClipEnable = FALSE;
		rasterizerDesc_.MultisampleEnable = FALSE;
		rasterizerDesc_.AntialiasedLineEnable = FALSE;
		rasterizerDesc_.ForcedSampleCount = 0;
		rasterizerDesc_.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	// RTBlend
	{
		renderTargetBlendStateDesc_ = {
			 FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
	}

	// blendState
	{
		blendDesc_.AlphaToCoverageEnable = FALSE;
		blendDesc_.IndependentBlendEnable = FALSE;
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {

			blendDesc_.RenderTarget[i] = renderTargetBlendStateDesc_;
		}
	}

	// depthStencilop
	{
		depthStencilopDesc_.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilopDesc_.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilopDesc_.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		depthStencilopDesc_.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}

	// depthStencil
	{
		depthStencilDesc_.DepthEnable = FALSE;
		depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc_.StencilEnable = FALSE;
		depthStencilDesc_.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc_.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		depthStencilDesc_.FrontFace = depthStencilopDesc_;
		depthStencilDesc_.BackFace = depthStencilopDesc_;
	}

	// shaderByteCode
	{
		msShaderByteCode_.BytecodeLength = sizeof(msShaderBlob_);
		msShaderByteCode_.pShaderBytecode = msShaderBlob_.Get();

		psShaderByteCode_.BytecodeLength = sizeof(psShaderBlob_);
		psShaderByteCode_.pShaderBytecode = psShaderBlob_.Get();
	}

	// descSample
	{
		sampleDesc_.Count = 1;
		sampleDesc_.Quality = 0;
	}

	// RTFormat
	{
		renderTargetFoamat_.NumRenderTargets = 1;
		renderTargetFoamat_.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}

	// pipelineState
	{
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipelineDesc{};

		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		pipelineDesc.SampleMask = UINT_MAX;

		pipelineDesc.pRootSignature = rootSignature_.Get();
		pipelineDesc.MS = { msShaderBlob_.Get()->GetBufferPointer(), msShaderBlob_.Get()->GetBufferSize() };
		pipelineDesc.PS = { psShaderBlob_.Get()->GetBufferPointer(), psShaderBlob_.Get()->GetBufferSize() };

		pipelineDesc.RasterizerState = rasterizerDesc_;
		pipelineDesc.BlendState = blendDesc_;
		pipelineDesc.DepthStencilState = depthStencilDesc_;
		pipelineDesc.SampleDesc = sampleDesc_;

		auto pipelineStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipelineDesc);

		D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
		streamDesc.pPipelineStateSubobjectStream = &pipelineStream;
		streamDesc.SizeInBytes = sizeof(pipelineStream);

		hr = device->CreatePipelineState(&streamDesc,
			IID_PPV_ARGS(pipelineState_.GetAddressOf()));
		if (FAILED(hr)) {
			ASSERT(FALSE, "assert createPipeline");
		}
	}
}