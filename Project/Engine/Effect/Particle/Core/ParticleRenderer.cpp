#include "ParticleRenderer.h"

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================
#include <Engine/Effect/Particle/Data/GPUParticleGroup.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Effect/Particle/ParticleConfig.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	ParticleRenderer classMethods
//============================================================================

void ParticleRenderer::Init(ID3D12Device8* device, SRVDescriptor* srvDescriptor,
	DxShaderCompiler* shaderCompiler) {

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

void ParticleRenderer::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	// データがなければ描画しない
	if (gpuGroups_.empty()) {
		return;
	}

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();

	const uint32_t typeIndex = static_cast<uint32_t>(ParticleType::GPU);
	for (auto& group : gpuGroups_) {

		const uint32_t primitiveIndex = static_cast<uint32_t>(group.GetPrimitiveType());

		// pipeline設定
		commandList->SetGraphicsRootSignature(pipelines_[typeIndex][primitiveIndex]->GetRootSignature());
		commandList->SetPipelineState(pipelines_[typeIndex][primitiveIndex]->GetGraphicsPipeline(group.GetBlendMode()));

		// primitive形状の型取得を描画形状から取得する
		// textureの設定がまだない
		// 発生タイマーの更新をまだやってない

		// 形状
		commandList->SetGraphicsRootShaderResourceView(0, group.GetPrimitiveBufferAdress());

		// transform

		// cBuffer

		// material

		// textureTable

		// 描画
		commandList->DispatchMesh(kMaxParticles, 1, 1);
	}
}

void ParticleRenderer::SetGPUGroup(const std::vector<GPUParticleGroup>& group) {

	if (group.empty()) {
		return;
	}

	gpuGroups_.clear();
	gpuGroups_ = group;
}