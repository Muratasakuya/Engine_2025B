#include "ParticleSystem.h"

//============================================================================
//	include
//============================================================================
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

void ParticleSystem::Init(Asset* asset, ID3D12Device* device, CameraManager* cameraManager) {

	asset_ = nullptr;
	asset_ = asset;

	device_ = nullptr;
	device_ = device;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	emitterHandler_ = std::make_unique<ParticleEmitterHandler>();
	emitterHandler_->Init();
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

	// 各emitterの更新
	Matrix4x4 billboardMatrix = cameraManager_->GetCamera()->GetBillboardMatrix();
	for (const auto& emitter : std::views::values(emitters_)) {

		emitter->Update(billboardMatrix);
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