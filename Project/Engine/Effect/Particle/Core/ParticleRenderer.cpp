#include "ParticleRenderer.h"

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================
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

	// 描画情報を流して描画
	debugEnable;
	sceneBuffer;
	dxCommand;
}