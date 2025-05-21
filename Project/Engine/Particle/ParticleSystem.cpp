#include "ParticleSystem.h"

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

void ParticleSystem::Init(Asset* asset, CameraManager* cameraManager) {

	asset_ = nullptr;
	asset_ = asset;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	emitterHandler_ = std::make_unique<ParticleEmitterHandler>();
	emitterHandler_->Init();
}

void ParticleSystem::Update() {

	// emitterの作成
	CreateEmitter();
}

void ParticleSystem::CreateEmitter() {

	// handlerから通知を受け取る
	// 追加通知
	if (emitterHandler_->IsAddEmitter()) {

		// 名前を取得
		const std::string& emitterName = emitterHandler_->GetAddEmitterName();

		// emitterを作成
		emitters_[emitterName] = std::make_unique<ParticleEmitter>();
		emitters_[emitterName]->Init(emitterName, asset_);

		// 追加し終わったのでフラグを元に戻す
		emitterHandler_->ClearNotification();
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