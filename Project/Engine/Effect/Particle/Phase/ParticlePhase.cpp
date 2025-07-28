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

	uint32_t index = static_cast<size_t>(id);

	// 作成していない場合のみ作成する
	if (!spawnerCache_[index]) {

		std::unique_ptr<ICPUParticleSpawnModule> module = SpawnRegistry::GetInstance().Create(id);
		module->SetAsset(asset_);
		module->SetPrimitiveType(primitiveType_);
		module->Init();

		// 現在有効な物があった場合、データを共有する
		// カウントは処理しない
		if (currentSpawnId_ != ParticleSpawnModuleID::Count) {
			if (spawnerCache_[static_cast<size_t>(currentSpawnId_)]) {

				module->ShareCommonParam(spawnerCache_[static_cast<size_t>(currentSpawnId_)].get());
			}
		}

		spawnerCache_[index] = std::move(module);
	}

	// 作成してあればキャッシュから引っ張ってきて使用する
	spawner_ = spawnerCache_[index].get();
	currentSpawnId_ = id;
}

void ParticlePhase::AddUpdater(ParticleUpdateModuleID id) {

	// 追加して初期化
	updaters_.push_back(UpdateRegistry::GetInstance().Create(id));
	updaters_.back()->Init();
}

void ParticlePhase::RemoveUpdater(uint32_t index) {

	updaters_.erase(updaters_.begin() + index);
}

void ParticlePhase::SwapUpdater(uint32_t from, uint32_t to) {

	if (from == to) {
		return;
	}
	if (from < to) {

		std::rotate(updaters_.begin() + from, updaters_.begin() + from + 1, updaters_.begin() + to + 1);
	} else {

		std::rotate(updaters_.begin() + to, updaters_.begin() + from, updaters_.begin() + from + 1);
	}
}

void ParticlePhase::ImGui() {

	ImGui::SeparatorText("Emit Duration");
	ImGui::Text("elapsed:%.3f / duration:%.3f", elapsed_, duration_);

	if (ImGui::BeginTabBar("ParticlePhase")) {

		//============================================================================
		//	Render
		//============================================================================
		if (ImGui::BeginTabItem("Render")) {

			spawner_->ImGuiRenderParam();

			ImGui::EndTabItem();
		}

		//============================================================================
		//	Emit
		//============================================================================
		if (ImGui::BeginTabItem("Emit")) {

			ImGui::DragFloat("duration", &duration_, 0.01f);

			spawner_->ImGuiEmitParam();

			ImGui::EndTabItem();
		}

		//============================================================================
		//	Spawners
		//============================================================================
		if (ImGui::BeginTabItem("Spawner")) {

			if (EnumAdapter<ParticleSpawnModuleID>::Combo("Spawner", &selectSpawnModule_)) {

				SetSpawner(selectSpawnModule_);
			}
			// 選択されている物を表示
			spawner_->ImGui();

			ImGui::EndTabItem();
		}

		//============================================================================
		//	Updaters
		//============================================================================
		if (ImGui::BeginTabItem("Updater")) {

			// 追加処理
			if (ImGui::Button("Add Updater")) {
				ImGui::OpenPopup("AddUpdater");
			}
			if (ImGui::BeginPopup("AddUpdater")) {
				for (int i = 0; i < EnumAdapter<ParticleUpdateModuleID>::GetEnumCount(); ++i) {
					if (ImGui::Selectable(EnumAdapter<ParticleUpdateModuleID>::GetEnumName(i))) {

						// 追加
						AddUpdater(EnumAdapter<ParticleUpdateModuleID>::GetValue(i));
						selectedUpdater_ = static_cast<int>(updaters_.size()) - 1;
					}
				}
				ImGui::EndPopup();
			}

			//============================================================================
			//	Updaters: Select
			//============================================================================

			ImGui::BeginChild("UpdatersSelect", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

			for (size_t i = 0; i < updaters_.size(); ++i) {

				ImGui::PushID(static_cast<int>(i));
				bool isSelected = (selectedUpdater_ == static_cast<int>(i));
				ImGui::Selectable(updaters_[i]->GetName(), isSelected, ImGuiSelectableFlags_AllowDoubleClick);

				if (ImGui::IsItemClicked()) {

					selectedUpdater_ = static_cast<int>(i);
				}

				// ドラッグ＆ドロップ
				if (ImGui::BeginDragDropSource()) {

					ImGui::SetDragDropPayload("UPD_IDX", &i, sizeof(size_t));
					ImGui::Text("%s", updaters_[i]->GetName());
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("UPD_IDX")) {

						size_t from = *static_cast<const size_t*>(p->Data);
						size_t to = i;
						if (from != to) {

							SwapUpdater(static_cast<uint32_t>(from), static_cast<uint32_t>(to));
						}
						selectedUpdater_ = static_cast<int>(to);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::PopID();
			}

			ImGui::EndChild();
			ImGui::SameLine();

			//============================================================================
			//	Updaters: Edit
			//============================================================================

			ImGui::BeginChild("UpdatersEdit", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

			// 選択中Updaterの値操作
			if (selectedUpdater_ >= 0 && selectedUpdater_ < static_cast<int>(updaters_.size())) {

				ImGui::SeparatorText(updaters_[selectedUpdater_]->GetName());

				updaters_[selectedUpdater_]->ImGui();
			}

			// Deleteで削除
			if (selectedUpdater_ >= 0 &&
				ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
				ImGui::IsKeyPressed(ImGuiKey_Delete)) {

				RemoveUpdater(static_cast<uint32_t>(selectedUpdater_));
				selectedUpdater_ = std::clamp(selectedUpdater_ - 1, -1, static_cast<int>(updaters_.size()) - 1);
			}

			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}