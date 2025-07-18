#include "ParticleManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
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
	pipelines_[ParticleShape::Plane] = std::make_unique<PipelineState>();
	pipelines_[ParticleShape::Plane]->Create("EffectPlane.json", device, srvDescriptor, shaderCompiler);

	pipelines_[ParticleShape::Ring] = std::make_unique<PipelineState>();
	pipelines_[ParticleShape::Ring]->Create("EffectRing.json", device, srvDescriptor, shaderCompiler);

	pipelines_[ParticleShape::Cylinder] = std::make_unique<PipelineState>();
	pipelines_[ParticleShape::Cylinder]->Create("EffectCylinder.json", device, srvDescriptor, shaderCompiler);

	const UINT kMaxInstanceCount = 4;

	// buffer作成
	planeBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount);
	ringBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount);
	cylinderBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount);
	materialBuffer_.CreateStructuredBuffer(device, kMaxInstanceCount);

	PlaneForGPU plane{};
	planeTransform_.Init();
	planeTransform_.UpdateMatrix();
	plane.size = Vector2::AnyInit(4.0f);
	plane.world = planeTransform_.matrix.world;

	planeInstances_.emplace_back(plane);

	RingForGPU ring{};
	ringTransform_.Init();
	ringTransform_.UpdateMatrix();
	ring.innerRadius = 2.0f;
	ring.outerRadius = 8.0f;
	ring.divide = 8;
	ring.world = ringTransform_.matrix.world;

	ringInstances_.emplace_back(ring);

	CylinderForGPU cylinder{};
	cylinderTransform_.Init();
	cylinderTransform_.UpdateMatrix();
	cylinder.topRadius = 4.0f;
	cylinder.bottomRadius = 4.0f;
	cylinder.height = 8.0f;
	cylinder.divide = 8;
	cylinder.world = cylinderTransform_.matrix.world;

	cylinderInstances_.emplace_back(cylinder);

	ParticleMaterial material{};
	material.color = Color::White();
	material.uvTransform = Matrix4x4::MakeIdentity4x4();

	materialInstances_.emplace_back(material);
}

void ParticleManager::Update() {

	// buffer転送
	planeBuffer_.TransferVectorData(planeInstances_);
	ringBuffer_.TransferVectorData(ringInstances_);
	cylinderBuffer_.TransferVectorData(cylinderInstances_);
	materialBuffer_.TransferVectorData(materialInstances_);
}

void ParticleManager::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, ID3D12GraphicsCommandList6* commandList) {

	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle = asset_->GetGPUHandle("uvChecker");
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle1 = asset_->GetGPUHandle("gradationLine_0");
	D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle2 = asset_->GetGPUHandle("gradationLine_1");

	{
		// plane描画
		// pipeline設定
		commandList->SetGraphicsRootSignature(pipelines_[ParticleShape::Plane]->GetRootSignature());
		commandList->SetPipelineState(pipelines_[ParticleShape::Plane]->GetGraphicsPipeline());

		// plane
		commandList->SetGraphicsRootShaderResourceView(0,
			planeBuffer_.GetResource()->GetGPUVirtualAddress());
		// viewPro
		sceneBuffer->SetViewProCommand(debugEnable, commandList, 1);
		// material
		commandList->SetGraphicsRootShaderResourceView(2,
			materialBuffer_.GetResource()->GetGPUVirtualAddress());
		// texture
		commandList->SetGraphicsRootDescriptorTable(3, textureGPUHandle);
		// 描画
		commandList->DispatchMesh(static_cast<UINT>(planeInstances_.size()), 1, 1);
	}
	{
		// ring描画
		// pipeline設定
		commandList->SetGraphicsRootSignature(pipelines_[ParticleShape::Ring]->GetRootSignature());
		commandList->SetPipelineState(pipelines_[ParticleShape::Ring]->GetGraphicsPipeline());

		// ring
		commandList->SetGraphicsRootShaderResourceView(0,
			ringBuffer_.GetResource()->GetGPUVirtualAddress());
		// viewPro
		sceneBuffer->SetViewProCommand(debugEnable, commandList, 1);
		// material
		commandList->SetGraphicsRootShaderResourceView(2,
			materialBuffer_.GetResource()->GetGPUVirtualAddress());
		// texture
		commandList->SetGraphicsRootDescriptorTable(3, textureGPUHandle2);
		// 描画
		commandList->DispatchMesh(static_cast<UINT>(ringInstances_.size()), 1, 1);
	}
	{
		// cylinder描画
		// pipeline設定
		commandList->SetGraphicsRootSignature(pipelines_[ParticleShape::Cylinder]->GetRootSignature());
		commandList->SetPipelineState(pipelines_[ParticleShape::Cylinder]->GetGraphicsPipeline(kBlendModeAdd));

		// cylinder
		commandList->SetGraphicsRootShaderResourceView(0,
			cylinderBuffer_.GetResource()->GetGPUVirtualAddress());
		// viewPro
		sceneBuffer->SetViewProCommand(debugEnable, commandList, 1);
		// material
		commandList->SetGraphicsRootShaderResourceView(2,
			materialBuffer_.GetResource()->GetGPUVirtualAddress());
		// texture
		commandList->SetGraphicsRootDescriptorTable(3, textureGPUHandle1);
		// 描画
		commandList->DispatchMesh(static_cast<UINT>(cylinderInstances_.size()), 1, 1);
	}
}

void ParticleManager::ImGui() {

	ImGui::SeparatorText("Plane");

	ImGui::PushID("Plane");

	planeTransform_.ImGui(224.0f);
	planeTransform_.UpdateMatrix();
	ImGui::DragFloat2("size##Plane", &planeInstances_[0].size.x, 0.01f);
	planeInstances_[0].world = planeTransform_.matrix.world;

	ImGui::PopID();

	ImGui::SeparatorText("Ring");

	ImGui::PushID("Ring");

	ringTransform_.ImGui(224.0f);
	ringTransform_.UpdateMatrix();
	ImGui::DragFloat("innerRadius##Ring", &ringInstances_[0].innerRadius, 0.01f);
	ImGui::DragFloat("outerRadius##Ring", &ringInstances_[0].outerRadius, 0.01f);
	ImGui::DragInt("divide##Ring", &ringInstances_[0].divide, 1.0f, 3, 32);
	ringInstances_[0].world = ringTransform_.matrix.world;

	ImGui::PopID();

	ImGui::SeparatorText("Cylinder");

	ImGui::PushID("Cylinder");

	cylinderTransform_.ImGui(224.0f);
	cylinderTransform_.UpdateMatrix();
	ImGui::DragFloat("topRadius##Cylinder", &cylinderInstances_[0].topRadius, 0.01f);
	ImGui::DragFloat("bottomRadius##Cylinder", &cylinderInstances_[0].bottomRadius, 0.01f);
	ImGui::DragFloat("height##Cylinder", &cylinderInstances_[0].height, 0.01f);
	ImGui::DragInt("divide##Cylinder", &cylinderInstances_[0].divide, 1.0f, 3, 32);
	cylinderInstances_[0].world = cylinderTransform_.matrix.world;

	ImGui::PopID();
}