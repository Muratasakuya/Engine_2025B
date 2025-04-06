#include "MeshShaderPipelineState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>
#include <Engine/Core/Graphics/Managers/SRVManager.h>

//============================================================================
//	MeshShaderPipelineState classMethods
//============================================================================

void MeshShaderPipelineState::Create(ID3D12Device8* device,
	DxShaderCompiler* shaderCompiler, SRVManager* srvManager) {

	// shaderCompile
	{
		shaderCompiler->CompileShader(
			L"./Assets/Engine/Shaders/MS/MeshStandard.MS.hlsl", L"ms_6_5", msShaderBlob_);
		shaderCompiler->CompileShader(
			L"./Assets/Engine/Shaders/MS/MeshStandard.PS.hlsl", L"ps_6_0", psShaderBlob_);
	}

	HRESULT hr = S_OK;

	// rootSignature作成
	{
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

		auto flag = D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		rootSignatureDesc.Flags = flag;

		D3D12_ROOT_PARAMETER rootParameters[13]{};

		//------------------------------------------------------------------------
		// meshShaderRoot

		// gVertices: SRV
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[0].Descriptor.ShaderRegister = 0;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// gIndices: SRV
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[1].Descriptor.ShaderRegister = 1;
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// gMeshlets: SRV
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[2].Descriptor.ShaderRegister = 2;
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// gPrimitives: SRV
		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[3].Descriptor.ShaderRegister = 3;
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// transform: SRV
		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[4].Descriptor.ShaderRegister = 4;
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// InstanceData: CBV
		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[5].Descriptor.ShaderRegister = 0;
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// CameraData: CBV
		rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[6].Descriptor.ShaderRegister = 1;
		rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;
		// ShadowLight: CBV
		rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[7].Descriptor.ShaderRegister = 2;
		rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_MESH;

		//------------------------------------------------------------------------
		// pixelShaderRoot

		D3D12_DESCRIPTOR_RANGE textureDescriptorRange[1]{};
		textureDescriptorRange[0].NumDescriptors = srvManager->GetMaxSRVCount();
		textureDescriptorRange[0].BaseShaderRegister = 1;
		textureDescriptorRange[0].RegisterSpace = 0;
		textureDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		textureDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_DESCRIPTOR_RANGE shadowMapDescriptorRange[1]{};
		shadowMapDescriptorRange[0].NumDescriptors = 1;
		shadowMapDescriptorRange[0].BaseShaderRegister = 2;
		shadowMapDescriptorRange[0].RegisterSpace = 1;
		shadowMapDescriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		shadowMapDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// gMaterials: SRV
		rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rootParameters[8].Descriptor.ShaderRegister = 0;
		rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		// gTexture: DESCRIPTOR_TABLE
		rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[9].DescriptorTable.pDescriptorRanges = textureDescriptorRange;
		rootParameters[9].DescriptorTable.NumDescriptorRanges = _countof(textureDescriptorRange);
		// gShadowTexture: DESCRIPTOR_TABLE
		rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[10].DescriptorTable.pDescriptorRanges = shadowMapDescriptorRange;
		rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(shadowMapDescriptorRange);
		// PunctualLight: CBV
		rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[11].Descriptor.ShaderRegister = 1;
		rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		// Camera: CBV
		rootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[12].Descriptor.ShaderRegister = 2;
		rootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// rootParameter設定
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;

		//------------------------------------------------------------------------
		// staticSampler

		D3D12_STATIC_SAMPLER_DESC staticSamplers[2]{};

		// texture
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[0].ShaderRegister = 0;
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// texture
		staticSamplers[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
		staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers[1].ShaderRegister = 1;
		staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// Sampler設定
		rootSignatureDesc.pStaticSamplers = staticSamplers;
		rootSignatureDesc.NumStaticSamplers = _countof(staticSamplers);

		ComPtr<ID3DBlob> signatureBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
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
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesc_.DepthClipEnable = FALSE;
		rasterizerDesc_.AntialiasedLineEnable = FALSE;
	}

	// RTBlend
	{
		/*renderTargetBlendStateDesc_.BlendEnable = true;
		renderTargetBlendStateDesc_.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		renderTargetBlendStateDesc_.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		renderTargetBlendStateDesc_.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetBlendStateDesc_.SrcBlendAlpha = D3D12_BLEND_ZERO;
		renderTargetBlendStateDesc_.DestBlendAlpha = D3D12_BLEND_ONE;
		renderTargetBlendStateDesc_.BlendOpAlpha = D3D12_BLEND_OP_ADD;*/
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
		depthStencilDesc_.DepthEnable = TRUE;
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