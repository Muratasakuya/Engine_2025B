#include "ParticleRenderer.h"

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Effect/Particle/Data/GPUParticleGroup.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Engine/Scene/SceneView.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ParticleRenderer classMethods
//============================================================================

void ParticleRenderer::Init(ID3D12Device8* device, Asset* asset,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	asset_ = nullptr;
	asset_ = asset;

	// 各pipeline初期化
	InitPipelines(device, srvDescriptor, shaderCompiler);
}

void ParticleRenderer::InitPipelines(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	for (uint32_t typeIndex = 0; typeIndex < kParticleTypeCount; ++typeIndex) {

		// タイプの名前を取得
		const char* typeName = EnumAdapter<ParticleType>::GetEnumName(typeIndex);
		for (uint32_t primitiveIndex = 0; primitiveIndex < kPrimitiveCount; ++primitiveIndex) {

			// 形状の名前を取得
			const char* shapeName = EnumAdapter<ParticlePrimitiveType>::GetEnumName(primitiveIndex);
			std::string jsonFile = std::string(typeName) + "Particle" + std::string(shapeName) + ".json";

			// 作成
			auto& pipeline = pipelines_[typeIndex][primitiveIndex];
			pipeline = std::make_unique<PipelineState>();
			pipeline->Create(jsonFile, device, srvDescriptor, shaderCompiler);
		}
	}
}

void ParticleRenderer::Rendering(bool debugEnable, const GPUParticleGroup& group,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();
	const uint32_t typeIndex = static_cast<uint32_t>(ParticleType::GPU);
	const uint32_t primitiveIndex = static_cast<uint32_t>(group.GetPrimitiveType());

	// pipeline設定
	commandList->SetGraphicsRootSignature(pipelines_[typeIndex][primitiveIndex]->GetRootSignature());
	commandList->SetPipelineState(pipelines_[typeIndex][primitiveIndex]->GetGraphicsPipeline(group.GetBlendMode()));

	// 形状
	commandList->SetGraphicsRootShaderResourceView(0, group.GetPrimitiveBufferAdress());
	// transform
	commandList->SetGraphicsRootShaderResourceView(1, group.GetTransformBuffer().GetResource()->GetGPUVirtualAddress());
	// perView
	sceneBuffer->SetPerViewCommand(debugEnable, commandList, 2);
	// material
	commandList->SetGraphicsRootShaderResourceView(3, group.GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());
	// textureTable
	commandList->SetGraphicsRootDescriptorTable(4, asset_->GetGPUHandle(group.GetTextureName()));
	// 描画
	commandList->DispatchMesh(kMaxParticles, 1, 1);

	//============================================================================
	// バリア遷移処理

#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	if (debugEnable) {

		// MeshShader -> ComputeShader
		dxCommand->TransitionBarriers({ group.GetTransformBuffer().GetResource() },
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// PixelShader -> ComputeShader
		dxCommand->TransitionBarriers({ group.GetMaterialBuffer().GetResource() },
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
#else

	// MeshShader -> ComputeShader
	dxCommand->TransitionBarriers({ group.GetTransformBuffer().GetResource() },
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// PixelShader -> ComputeShader
	dxCommand->TransitionBarriers({ group.GetMaterialBuffer().GetResource() },
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
#endif
}