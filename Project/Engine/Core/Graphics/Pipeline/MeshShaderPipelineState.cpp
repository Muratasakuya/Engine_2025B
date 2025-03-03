#include "MeshShaderPipelineState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>

namespace {

	//============================================================================
	//	MeshShaderStateParam class
	//============================================================================
	template <typename ValueType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE ObjectType>
	class MeshShaderStateParam {
	public:
		//========================================================================
		//	public Methods
		//========================================================================

		MeshShaderStateParam() :type_(ObjectType), value_(ValueType()) {}
		MeshShaderStateParam(const ValueType& value) :type_(ObjectType), value_(value) {}

		~MeshShaderStateParam() = default;

		MeshShaderStateParam& operator=(const ValueType& value) {

			value_ = value;
			type_ = ObjectType;
			return *this;
		}

	private:
		//========================================================================
		//	private Methods
		//========================================================================

		//--------- variables ----------------------------------------------------

		ValueType value_;
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_;
	};

	// 省略形
#define PSST(x) D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_##x
	using SP_ROOT_SIGNATURE = MeshShaderStateParam<ID3D12RootSignature*, PSST(ROOT_SIGNATURE)>;
	using SP_AS = MeshShaderStateParam<D3D12_SHADER_BYTECODE, PSST(AS)>;
	using SP_MS = MeshShaderStateParam<D3D12_SHADER_BYTECODE, PSST(MS)>;
	using SP_PS = MeshShaderStateParam<D3D12_SHADER_BYTECODE, PSST(PS)>;
	using SP_BLEND = MeshShaderStateParam<D3D12_BLEND_DESC, PSST(BLEND)>;
	using SP_RASTERIZER = MeshShaderStateParam<D3D12_RASTERIZER_DESC, PSST(RASTERIZER)>;
	using SP_DEPTH_STENCIL = MeshShaderStateParam<D3D12_DEPTH_STENCIL_DESC, PSST(DEPTH_STENCIL)>;
	using SP_SAMPLE_MASK = MeshShaderStateParam<UINT, PSST(SAMPLE_MASK)>;
	using SP_SAMPLE_DESC = MeshShaderStateParam<DXGI_SAMPLE_DESC, PSST(SAMPLE_DESC)>;
	using SP_RT_FORMAT = MeshShaderStateParam<D3D12_RT_FORMAT_ARRAY, PSST(RENDER_TARGET_FORMATS)>;
	using SP_DS_FORMAT = MeshShaderStateParam<DXGI_FORMAT, PSST(DEPTH_STENCIL_FORMAT)>;
	using SP_FLAGS = MeshShaderStateParam<D3D12_PIPELINE_STATE_FLAGS, PSST(FLAGS)>;
#undef PSST

	struct MeshShaderPipelineStateDesc {

		SP_ROOT_SIGNATURE  RootSignature;
		SP_AS              AS; // AmplificationShader
		SP_MS              MS; // MeshShader
		SP_PS              PS; // PixelShader
		SP_BLEND           Blend;        // Blend
		SP_RASTERIZER      Rasterizer;   // RasterizerState
		SP_DEPTH_STENCIL   DepthStencil; // DepthStencil
		SP_SAMPLE_MASK     SampleMask;   // SampleMask
		SP_SAMPLE_DESC     SampleDesc;   // Sample
		SP_RT_FORMAT       RTFormats;    // RTV
		SP_DS_FORMAT       DSFormat;     // DSFormat
		SP_FLAGS           Flags;        // Flag
	};
}

//============================================================================
//	MeshShaderPipelineState classMethods
//============================================================================

void MeshShaderPipelineState::Create(ID3D12Device8* device, DxShaderCompiler* shaderCompiler) {

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
		MeshShaderPipelineStateDesc pipelineState{};
		pipelineState.RootSignature = rootSignature_.Get();
		pipelineState.MS = msShaderByteCode_;
		pipelineState.PS = psShaderByteCode_;
		pipelineState.Rasterizer = rasterizerDesc_;
		pipelineState.Blend = blendDesc_;
		pipelineState.DepthStencil = depthStencilDesc_;
		pipelineState.SampleMask = UINT_MAX;
		pipelineState.SampleDesc = sampleDesc_;
		pipelineState.RTFormats = renderTargetFoamat_;
		pipelineState.DSFormat = DXGI_FORMAT_UNKNOWN;
		pipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStream{};
		pipelineStateStream.SizeInBytes = sizeof(pipelineState);
		pipelineStateStream.pPipelineStateSubobjectStream = &pipelineState;

		hr = device->CreatePipelineState(&pipelineStateStream,
			IID_PPV_ARGS(pipelineState_.GetAddressOf()));
		if (FAILED(hr)) {
			ASSERT(FALSE, "assert createPipeline");
		}
	}
}