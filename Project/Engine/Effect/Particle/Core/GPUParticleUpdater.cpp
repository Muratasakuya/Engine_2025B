#include "GPUParticleUpdater.h"

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================

void GPUParticleUpdater::Init(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// 各pipeline初期化
	InitPipelines(device, srvDescriptor, shaderCompiler);
}

void GPUParticleUpdater::InitPipelines(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	initPipeline_ = std::make_unique<PipelineState>();
	initPipeline_->Create("InitParticle.json", device, srvDescriptor, shaderCompiler);

	updatePipeline_ = std::make_unique<PipelineState>();
	updatePipeline_->Create("UpdateParticle.json", device, srvDescriptor, shaderCompiler);

	for (uint32_t index = 0; index < kEmitterShapeCount; ++index) {

		// 形状の名前を取得
		const char* shapeName = EnumAdapter<ParticleEmitterShape>::GetEnumName(index);
		std::string jsonFile = "Emit" + std::string(shapeName) + "Particle.json";

		auto& pipeline = emitPipelines_[index];
		pipeline = std::make_unique<PipelineState>();
		pipeline->Create(jsonFile, device, srvDescriptor, shaderCompiler);
	}
}

void GPUParticleUpdater::DispatchInit(
	const GPUParticleGroup& group, DxCommand* dxCommand) {

}

void GPUParticleUpdater::DispatchEmit(
	const GPUParticleGroup& group, DxCommand* dxCommand) {


}

void GPUParticleUpdater::DispatchUpdate(
	const GPUParticleGroup& group, DxCommand* dxCommand) {


}
