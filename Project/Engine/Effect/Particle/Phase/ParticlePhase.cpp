#include "ParticlePhase.h"

//============================================================================
//	ParticlePhase classMethods
//============================================================================

void ParticlePhase::Init(Asset* asset, ParticlePrimitiveType primitiveType) {

	asset_ = nullptr;
	asset_ = asset;

	primitiveType_ = primitiveType;

	// 初期化値
	duration_ = 0.8f;

	// 基本的なモジュールは設定
	AddUpdater(ParticleUpdateModuleID::Velocity);
	AddUpdater(ParticleUpdateModuleID::Rotation);
	AddUpdater(ParticleUpdateModuleID::Scale);
	AddUpdater(ParticleUpdateModuleID::Color);
}

void ParticlePhase::Emit(std::list<CPUParticle::ParticleData>& particles, float deltaTime) {

	// 時間経過を進める
	elapsed_ += deltaTime;
	// 時間を超えたら発生させる
	if (elapsed_ <= duration_) {
		return;
	}

	// 発生処理
	spawner_->Execute(particles);
	elapsed_ = 0.0f;
}

void ParticlePhase::UpdateParticle(CPUParticle::ParticleData& particle, float deltaTime) {

	// 更新処理
	for (const auto& updater : updaters_) {

		updater->Execute(particle, deltaTime);
	}	
}

void ParticlePhase::UpdateEmitter() {

	// emitterの更新
	if (!spawner_) {
		return;
	}
	spawner_->UpdateEmitter();
	spawner_->DrawEmitter();
}

void ParticlePhase::SetSpawner(ParticleSpawnModuleID id) {

	if (spawner_) {
		spawner_.reset();
	}
	spawner_ = SpawnRegistry::GetInstance().Create(id);

	// 必要なデータを設定
	spawner_->SetAsset(asset_);
	spawner_->SetPrimitiveType(primitiveType_);

	// 初期化
	spawner_->Init();
}

void ParticlePhase::AddUpdater(ParticleUpdateModuleID id) {

	updaters_.push_back(UpdateRegistry::GetInstance().Create(id));
}

void ParticlePhase::RemoveUpdater(uint32_t index) {

	updaters_.erase(updaters_.begin() + index);
}

void ParticlePhase::SwapUpdater(uint32_t from, uint32_t to) {

	std::rotate(updaters_.begin() + from, updaters_.begin() + from + 1, updaters_.begin() + to + 1);
}

void ParticlePhase::ImGui() {

	//============================================================================
	//	Emit
	//============================================================================

	ImGui::Text("elapsed:%.3f / duration:%.3f", elapsed_, duration_);
	ImGui::DragFloat("duration", &duration_, 0.01f);

	//============================================================================
	//	Spawners
	//============================================================================

	ImGui::SeparatorText("Spawners");

	// 発生モジュール選択
	if (EnumAdapter<ParticleSpawnModuleID>::Combo("Spawner", &selectSpawnModule_)) {

		SetSpawner(selectSpawnModule_);
	}
	if (spawner_) {

		spawner_->ImGui();
	} else {

		ImGui::Text("no selected spawner");
	}

	//============================================================================
	//	Updaters
	//============================================================================

	ImGui::SeparatorText("Updaters");

	if (ImGui::Button("+")) {

		ImGui::OpenPopup("AddUpdater");
	}

	if (ImGui::BeginPopup("AddUpdater")) {
		for (int i = 0; i < EnumAdapter<ParticleUpdateModuleID>::GetEnumCount(); ++i) {
			if (ImGui::Selectable(EnumAdapter<ParticleUpdateModuleID>::GetEnumName(i))) {

				AddUpdater(EnumAdapter<ParticleUpdateModuleID>::GetValue(i));
			}
		}
		ImGui::EndPopup();
	}

	for (size_t i = 0; i < updaters_.size(); ++i) {

		ImGui::PushID(static_cast<int>(i));
		ImGui::Selectable(updaters_[i]->GetName());
		ImGui::PopID();
	}
}