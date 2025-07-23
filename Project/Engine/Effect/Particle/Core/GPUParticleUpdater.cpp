#include "GPUParticleUpdater.h"

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================
#include <Engine/Effect/Particle/Data/GPUParticleGroup.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	GPUParticleUpdater classMethods
//============================================================================

void GPUParticleUpdater::Init(ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// 初期化
	perFrame_.time = 0.0f;
	perFrame_.deltaTime = 0.0f;
	// buffer作成
	perFrameBuffer_.CreateBuffer(device);

	// 各pipeline初期化
	InitPipelines(device, srvDescriptor, shaderCompiler);
}

void GPUParticleUpdater::Update(const GPUParticleGroup& group, DxCommand* dxCommand) {

	// 時間の更新
	BeginUpdate();

	// 各GPU処理
	// 初期化
	DispatchInit(group, dxCommand);
	// 発生
	DispatchEmit(group, dxCommand);
	// 更新
	DispatchUpdate(group, dxCommand);
}

void GPUParticleUpdater::BeginUpdate() {

	// 時間の更新
	perFrame_.deltaTime = GameTimer::GetDeltaTime();
	perFrame_.time += perFrame_.deltaTime;

	// buffer転送
	perFrameBuffer_.TransferData(perFrame_);
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

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();

	// 初期化用のpipeline設定
	commandList->SetComputeRootSignature(initPipeline_->GetRootSignature());
	commandList->SetPipelineState(initPipeline_->GetComputePipeline());

	// particle
	commandList->SetComputeRootUnorderedAccessView(0, group.GetParticleBuffer().GetResource()->GetGPUVirtualAddress());
	// transform
	commandList->SetComputeRootUnorderedAccessView(1, group.GetTransformBuffer().GetResource()->GetGPUVirtualAddress());
	// material
	commandList->SetComputeRootUnorderedAccessView(2, group.GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());
	// freeListIndex
	commandList->SetComputeRootUnorderedAccessView(3, group.GetFreeListIndexBuffer().GetResource()->GetGPUVirtualAddress());
	// freeList
	commandList->SetComputeRootUnorderedAccessView(4, group.GetFreeListBuffer().GetResource()->GetGPUVirtualAddress());

	// 実行
	commandList->Dispatch(1, 1, 1);

	// 各バッファのUAVバリア
	dxCommand->UAVBarrierAll();
}

void GPUParticleUpdater::DispatchEmit(
	const GPUParticleGroup& group, DxCommand* dxCommand) {

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();
	const uint32_t shapeIndex = static_cast<uint32_t>(group.GetEmitterShape());

	// 発生用のpipeline設定
	commandList->SetComputeRootSignature(emitPipelines_[shapeIndex]->GetRootSignature());
	commandList->SetPipelineState(emitPipelines_[shapeIndex]->GetComputePipeline());

	// particle
	commandList->SetComputeRootUnorderedAccessView(0, group.GetParticleBuffer().GetResource()->GetGPUVirtualAddress());
	// transform
	commandList->SetComputeRootUnorderedAccessView(1, group.GetTransformBuffer().GetResource()->GetGPUVirtualAddress());
	// material
	commandList->SetComputeRootUnorderedAccessView(2, group.GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());
	// freeListIndex
	commandList->SetComputeRootUnorderedAccessView(3, group.GetFreeListIndexBuffer().GetResource()->GetGPUVirtualAddress());
	// freeList
	commandList->SetComputeRootUnorderedAccessView(4, group.GetFreeListBuffer().GetResource()->GetGPUVirtualAddress());
	// emitterCommon
	commandList->SetComputeRootConstantBufferView(5, group.GetEmitterCommonBuffer().GetResource()->GetGPUVirtualAddress());
	// emitterShape
	commandList->SetComputeRootConstantBufferView(6, group.GetEmitterShapeBufferAdress());
	// perFrame
	commandList->SetComputeRootConstantBufferView(7, perFrameBuffer_.GetResource()->GetGPUVirtualAddress());

	// 実行
	commandList->Dispatch(1, 1, 1);

	// 各バッファのUAVバリア
	dxCommand->UAVBarrierAll();
}

void GPUParticleUpdater::DispatchUpdate(
	const GPUParticleGroup& group, DxCommand* dxCommand) {

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();

	// 更新用のpipeline設定
	commandList->SetComputeRootSignature(updatePipeline_->GetRootSignature());
	commandList->SetPipelineState(updatePipeline_->GetComputePipeline());

	// particle
	commandList->SetComputeRootUnorderedAccessView(0, group.GetParticleBuffer().GetResource()->GetGPUVirtualAddress());
	// transform
	commandList->SetComputeRootUnorderedAccessView(1, group.GetTransformBuffer().GetResource()->GetGPUVirtualAddress());
	// material
	commandList->SetComputeRootUnorderedAccessView(2, group.GetMaterialBuffer().GetResource()->GetGPUVirtualAddress());
	// freeListIndex
	commandList->SetComputeRootUnorderedAccessView(3, group.GetFreeListIndexBuffer().GetResource()->GetGPUVirtualAddress());
	// freeList
	commandList->SetComputeRootUnorderedAccessView(4, group.GetFreeListBuffer().GetResource()->GetGPUVirtualAddress());
	// perFrame
	commandList->SetComputeRootConstantBufferView(5, perFrameBuffer_.GetResource()->GetGPUVirtualAddress());

	// 実行
	commandList->Dispatch(1, 1, 1);

	//============================================================================
	// バリア遷移処理

	// ComputeShader -> MeshShader
	dxCommand->TransitionBarriers({ group.GetTransformBuffer().GetResource() },
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// ComputeShader -> PixelShader
	dxCommand->TransitionBarriers({ group.GetMaterialBuffer().GetResource() },
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}