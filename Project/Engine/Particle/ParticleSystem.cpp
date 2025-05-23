#include "ParticleSystem.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>
#include <Engine/Scene/Camera/CameraManager.h>

//============================================================================
//	ParticleSystem classMethods
//============================================================================

ParticleSystem* ParticleSystem::instance_ = nullptr;

ParticleSystem* ParticleSystem::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ParticleSystem();
	}
	return instance_;
}

void ParticleSystem::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ParticleSystem::Init(Asset* asset, ID3D12Device8* device,
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler,
	CameraManager* cameraManager) {

	asset_ = nullptr;
	asset_ = asset;

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	device_ = nullptr;
	device_ = device;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	emitterHandler_ = std::make_unique<ParticleEmitterHandler>();
	emitterHandler_->Init();

	// pipeline作成
	pipeline_ = std::make_unique<PipelineState>();
	pipeline_->Create("EffectMesh.json", device, srvDescriptor, shaderCompiler);
}

void ParticleSystem::Update() {

	// emitterの作成
	CreateEmitter();

	// 各emitterの更新処理
	UpdateEmitter();
}

void ParticleSystem::CreateEmitter() {

	// handlerから通知を受け取る
	// 追加通知
	if (emitterHandler_->IsAddEmitter()) {

		// 名前を取得
		const std::string& emitterName = emitterHandler_->GetAddEmitterName();

		// emitterを作成
		emitters_[emitterName] = std::make_unique<ParticleEmitter>();
		emitters_[emitterName]->Init(emitterName, asset_, device_);

		// 追加し終わったのでフラグを元に戻す
		emitterHandler_->ClearNotification();
	}
}

void ParticleSystem::UpdateEmitter() {

	if (emitters_.empty()) {
		return;
	}

	// renderDataをクリア
	renderParticleData_.clear();

	// 各emitterの更新
	Matrix4x4 billboardMatrix = cameraManager_->GetDebugCamera()->GetBillboardMatrix();
	for (const auto& emitter : std::views::values(emitters_)) {

		emitter->Update(billboardMatrix);

		// mapに追加
		for (const auto& particleGroup : emitter->GetParticleGroup()) {

			RenderParticleData data{};
			// mesh、bufferデータを作成
			data.mesh = particleGroup.mesh.get();
			data.materialBuffer = particleGroup.materialBuffer;
			data.worldMatrixBuffer = particleGroup.worldMatrixBuffer;
			// インスタンス数を設定
			data.numInstance = particleGroup.numInstance;
			renderParticleData_[particleGroup.parameter.blendMode].emplace_back(data);
		}
	}
}

void ParticleSystem::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, ID3D12GraphicsCommandList6* commandList) {

	// particleがなければ処理しない
	if (renderParticleData_.empty()) {
		return;
	}

	MeshCommandContext commandContext{};
	for (const auto& [blendMode, data] : renderParticleData_) {

		// pipeline設定
		commandList->SetGraphicsRootSignature(pipeline_->GetRootSignature());
		commandList->SetPipelineState(pipeline_->GetGraphicsPipeline(blendMode));

		// srv
		commandList->SetGraphicsRootDescriptorTable(8,
			srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
		// viewPro
		sceneBuffer->SetViewProCommand(debugEnable, commandList, 6);

		// particle描画
		for (const auto& bufferData : data) {

			// buffer設定
			// matrix
			commandList->SetGraphicsRootShaderResourceView(4,
				bufferData.worldMatrixBuffer.GetResource()->GetGPUVirtualAddress());
			// material
			commandList->SetGraphicsRootShaderResourceView(7,
				bufferData.materialBuffer.GetResource()->GetGPUVirtualAddress());

			// 描画処理
			commandContext.DispatchMesh(commandList, bufferData.numInstance, bufferData.mesh);
		}
	}
}

void ParticleSystem::ImGui() {

	// 追加、選択、削除処理
	emitterHandler_->ImGui(itemWidth_);

	// handlerでemitterを選択したらその名前のemitterを表示する
	const auto& selectEmitterName = emitterHandler_->GetSelectEmitterName();
	if (selectEmitterName.has_value()) {

		// emittterの操作
		emitters_[*selectEmitterName]->ImGui();
	}
}