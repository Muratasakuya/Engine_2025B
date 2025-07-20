#include "ParticleManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Utility/GameTimer.h>

//============================================================================
//	ParticleManager classMethods
//============================================================================

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ParticleManager::Init(Asset* asset, ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler, SceneView* sceneView) {

	asset_ = nullptr;
	asset_ = asset;

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	// pipeline作成
	// 計算
	initParticlePipeline_ = std::make_unique<PipelineState>();
	initParticlePipeline_->Create("InitParticle.json", device, srvDescriptor, shaderCompiler);
	emitParticlePipeline_ = std::make_unique<PipelineState>();
	emitParticlePipeline_->Create("EmitParticle.json", device, srvDescriptor, shaderCompiler);
	updateParticlePipeline_ = std::make_unique<PipelineState>();
	updateParticlePipeline_->Create("UpdateParticle.json", device, srvDescriptor, shaderCompiler);
	// 描画
	renderPipeline_ = std::make_unique<PipelineState>();
	renderPipeline_->Create("EffectPlane.json", device, srvDescriptor, shaderCompiler);

	// buffer作成
	planeBuffer_.CreateSRVBuffer(device, kMaxInstanceCount_);
	materialBuffer_.CreateSRVBuffer(device, kMaxInstanceCount_);
	particleBuffer_.CreateUAVBuffer(device, kMaxInstanceCount_);
	freeListIndexBuffer_.CreateUAVBuffer(device, 1);
	freeListBuffer_.CreateUAVBuffer(device, kMaxInstanceCount_);
	emitterSphereBuffer_.CreateBuffer(device);
	perFrameBuffer_.CreateBuffer(device);
	// SRV作成
	uint32_t srvIndex = 0;
	{
		srvDescriptor->CreateSRV(srvIndex, planeBuffer_.GetResource(),
			planeBuffer_.GetSRVDesc(kMaxInstanceCount_));
		planeBuffer_.SetSRVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));
	}
	{
		srvDescriptor->CreateSRV(srvIndex, materialBuffer_.GetResource(),
			materialBuffer_.GetSRVDesc(kMaxInstanceCount_));
		materialBuffer_.SetSRVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));
	}
	{
		srvDescriptor->CreateSRV(srvIndex, particleBuffer_.GetResource(),
			particleBuffer_.GetSRVDesc(kMaxInstanceCount_));
		particleBuffer_.SetSRVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));

		// UAVも作成する
		srvDescriptor->CreateUAV(srvIndex, particleBuffer_.GetResource(),
			particleBuffer_.GetUAVDesc(kMaxInstanceCount_));
		particleBuffer_.SetUAVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));
	}
	{
		// UAVを作成する
		srvDescriptor->CreateUAV(srvIndex, freeListIndexBuffer_.GetResource(),
			freeListIndexBuffer_.GetUAVDesc(1));
		freeListIndexBuffer_.SetUAVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));

		srvDescriptor->CreateUAV(srvIndex, freeListBuffer_.GetResource(),
			freeListBuffer_.GetUAVDesc(kMaxInstanceCount_));
		freeListBuffer_.SetUAVGPUHandle(srvDescriptor->GetGPUHandle(srvIndex));
	}

	// 最大数分だけ用意
	for (uint32_t index = 0; index < kMaxInstanceCount_; ++index) {

		PlaneForGPU plane{};
		plane.size = Vector2::AnyInit(4.0f);
		planeInstances_.emplace_back(plane);

		ParticleMaterial material{};
		material.color = Color::White();

		materialInstances_.emplace_back(material);
	}

	// emitter
	emitterSphere_.count = 16;
	emitterSphere_.frequency = 0.4f;
	emitterSphere_.frequencyTime = 0.0f;
	emitterSphere_.translation = Vector3::AnyInit(0.0f);
	emitterSphere_.radius = 8.0f;
}

void ParticleManager::InitParticle(DxCommand* dxCommand) {

	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// 初期化用pipeline
	commandList->SetComputeRootSignature(initParticlePipeline_->GetRootSignature());
	commandList->SetPipelineState(initParticlePipeline_->GetComputePipeline());

	// particle
	commandList->SetComputeRootDescriptorTable(0, particleBuffer_.GetUAVGPUHandle());
	// freeListIndex
	commandList->SetComputeRootDescriptorTable(1, freeListIndexBuffer_.GetUAVGPUHandle());
	// freeList
	commandList->SetComputeRootDescriptorTable(2, freeListBuffer_.GetUAVGPUHandle());

	// 実行処理
	commandList->Dispatch(1, 1, 1);
}

void ParticleManager::EmitParticle(DxCommand* dxCommand) {

	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// 発生用pipeline
	commandList->SetComputeRootSignature(emitParticlePipeline_->GetRootSignature());
	commandList->SetPipelineState(emitParticlePipeline_->GetComputePipeline());

	// particle
	commandList->SetComputeRootDescriptorTable(0, particleBuffer_.GetUAVGPUHandle());
	// freeListIndex
	commandList->SetComputeRootDescriptorTable(1, freeListIndexBuffer_.GetUAVGPUHandle());
	// freeList
	commandList->SetComputeRootDescriptorTable(2, freeListBuffer_.GetUAVGPUHandle());
	// emitterSphere
	commandList->SetComputeRootConstantBufferView(3, emitterSphereBuffer_.GetResource()->GetGPUVirtualAddress());
	// perFrame
	commandList->SetComputeRootConstantBufferView(4, perFrameBuffer_.GetResource()->GetGPUVirtualAddress());

	// 実行処理
	commandList->Dispatch(1, 1, 1);
}

void ParticleManager::UpdateParticle(DxCommand* dxCommand) {

	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// 更新用pipeline
	commandList->SetComputeRootSignature(updateParticlePipeline_->GetRootSignature());
	commandList->SetPipelineState(updateParticlePipeline_->GetComputePipeline());
	// particle
	commandList->SetComputeRootDescriptorTable(0, particleBuffer_.GetUAVGPUHandle());
	// freeListIndex
	commandList->SetComputeRootDescriptorTable(1, freeListIndexBuffer_.GetUAVGPUHandle());
	// freeList
	commandList->SetComputeRootDescriptorTable(2, freeListBuffer_.GetUAVGPUHandle());
	// perFrame
	commandList->SetComputeRootConstantBufferView(3, perFrameBuffer_.GetResource()->GetGPUVirtualAddress());

	// 実行処理
	commandList->Dispatch(1, 1, 1);

	// バリア処理
	// ComputeShader -> MeshShader
	dxCommand->TransitionBarriers({ particleBuffer_.GetResource() },
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void ParticleManager::Update(DxCommand* dxCommand) {

	perFrame_.deltaTime = GameTimer::GetDeltaTime();
	perFrame_.time += perFrame_.deltaTime;

	// buffer転送
	planeBuffer_.TransferData(planeInstances_);
	materialBuffer_.TransferData(materialInstances_);
	emitterSphereBuffer_.TransferData(emitterSphere_);
	perFrameBuffer_.TransferData(perFrame_);

	UpdateEmitter();
	EmitParticle(dxCommand);

	// UAVバリア
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = particleBuffer_.GetResource();
	dxCommand->GetCommandList()->ResourceBarrier(1, &barrier);

	UpdateParticle(dxCommand);
}

void ParticleManager::UpdateEmitter() {

	emitterSphere_.frequencyTime += GameTimer::GetDeltaTime();
	if (emitterSphere_.frequency <= emitterSphere_.frequencyTime) {

		emitterSphere_.frequencyTime -= emitterSphere_.frequency;
		emitterSphere_.emit = true;
	} else {

		emitterSphere_.emit = false;
	}
}

void ParticleManager::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	ID3D12GraphicsCommandList6* commandList = dxCommand->GetCommandList();
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle = asset_->GetGPUHandle("uvChecker");

	{
		// plane描画
		// pipeline設定
		commandList->SetGraphicsRootSignature(renderPipeline_->GetRootSignature());
		commandList->SetPipelineState(renderPipeline_->GetGraphicsPipeline(kBlendModeAdd));

		// plane
		commandList->SetGraphicsRootDescriptorTable(0, planeBuffer_.GetSRVGPUHandle());
		// particle
		commandList->SetGraphicsRootDescriptorTable(1, particleBuffer_.GetSRVGPUHandle());
		// perView
		sceneBuffer->SetPerViewCommand(debugEnable, commandList, 2);
		// material
		commandList->SetGraphicsRootDescriptorTable(3, materialBuffer_.GetSRVGPUHandle());
		// texture
		commandList->SetGraphicsRootDescriptorTable(4, textureGPUHandle);
		// 描画
		commandList->DispatchMesh(kMaxInstanceCount_, 1, 1);
	}

	if (debugEnable) {

		// バリア処理
		// MeshShader -> ComputeShader
		dxCommand->TransitionBarriers({ particleBuffer_.GetResource() },
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

void ParticleManager::ImGui() {

	ImGui::SeparatorText("Plane");

	ImGui::PushID("Plane");

	ImGui::DragFloat2("size##Plane", &planeInstances_[0].size.x, 0.01f);

	ImGui::PopID();
}