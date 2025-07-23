#include "ParticleManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/DxObject/DxCommand.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Effect/Particle/ParticleConfig.h>
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
	SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler) {

	// 各メインモジュール初期化
	gpuUpdater_ = std::make_unique<GPUParticleUpdater>();
	gpuUpdater_->Init(device, srvDescriptor, shaderCompiler);

	renderer_ = std::make_unique<ParticleRenderer>();
	renderer_->Init(device, asset, srvDescriptor, shaderCompiler);
}

void ParticleManager::Update(DxCommand* dxCommand) {

	if (systems_.empty()) {
		return;
	}

	// すべてのシステムを更新
	for (const auto& system : systems_) {
		for (const auto& group : system->GetGPUGroup()) {

			// GPU更新
			gpuUpdater_->Update(group, dxCommand);
		}
	}
}

void ParticleManager::Rendering(bool debugEnable,
	SceneConstBuffer* sceneBuffer, DxCommand* dxCommand) {

	if (systems_.empty()) {
		return;
	}

	// すべてのシステムを更新
	for (const auto& system : systems_) {
		for (const auto& group : system->GetGPUGroup()) {

			// 描画処理
			renderer_->Rendering(debugEnable, group, sceneBuffer, dxCommand);
		}
	}
}

void ParticleManager::ImGui() {
}