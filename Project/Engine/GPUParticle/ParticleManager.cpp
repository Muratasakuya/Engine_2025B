#include "ParticleManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Scene/SceneView.h>

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
	// 描画
	renderPipeline_ = std::make_unique<PipelineState>();
	renderPipeline_->Create("EffectPlane.json", device, srvDescriptor, shaderCompiler);

	// buffer作成
	planeBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount_);
	materialBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount_);
	particleBuffer_.CreateUavStructuredBuffer(device, kMaxInstanceCount_);

	// 最大数分だけ用意
	for (uint32_t index = 0; index < kMaxInstanceCount_; ++index) {

		PlaneForGPU plane{};
		plane.size = Vector2::AnyInit(4.0f);
		planeInstances_.emplace_back(plane);

		ParticleMaterial material{};
		material.color = Color::White();

		materialInstances_.emplace_back(material);
	}

}

void ParticleManager::InitParticle(DxCommand* dxCommand) {

	ID3D12GraphicsCommandList* commandList = dxCommand->GetCommandList();

	// 初期化用pipeline
	commandList->SetComputeRootSignature(initParticlePipeline_->GetRootSignature());
	commandList->SetPipelineState(initParticlePipeline_->GetComputePipeline());

	// particle
	commandList->SetComputeRootUnorderedAccessView(0,
		particleBuffer_.GetResource()->GetGPUVirtualAddress());

	// 実行処理
	commandList->Dispatch(1, 1, 1);

	// バリア処理
	// ComputeShader -> MeshShader
	dxCommand->TransitionBarriers({ particleBuffer_.GetResource() },
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
}

void ParticleManager::Update() {

	// buffer転送
	planeBuffer_.TransferVectorData(planeInstances_);
	materialBuffer_.TransferVectorData(materialInstances_);
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
		commandList->SetGraphicsRootShaderResourceView(0,
			planeBuffer_.GetResource()->GetGPUVirtualAddress());
		// particle
		commandList->SetGraphicsRootShaderResourceView(1,
			particleBuffer_.GetResource()->GetGPUVirtualAddress());
		// perView
		sceneBuffer->SetPerViewCommand(debugEnable, commandList, 2);
		// material
		commandList->SetGraphicsRootShaderResourceView(3,
			materialBuffer_.GetResource()->GetGPUVirtualAddress());
		// texture
		commandList->SetGraphicsRootDescriptorTable(4, textureGPUHandle);
		// 描画
		commandList->DispatchMesh(kMaxInstanceCount_, 1, 1);
	}
}

void ParticleManager::ImGui() {

	ImGui::SeparatorText("Plane");

	ImGui::PushID("Plane");

	ImGui::DragFloat2("size##Plane", &planeInstances_[0].size.x, 0.01f);

	ImGui::PopID();
}