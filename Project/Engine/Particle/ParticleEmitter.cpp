#include "ParticleEmitter.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	ParticleEmitter classMethods
//============================================================================

void ParticleEmitter::Init(const std::string& name, Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// 名前を設定
	name_ = name;

	// 初期化処理
	transform_.Init();

	// layout
	leftChildSize_ = ImVec2(320.0f, 148.0f);
	rightChildSize_ = ImVec2(380.0f, 148.0f);
	addButtonSize_ = ImVec2(208.0f, 30.0f);
	dropSize_ = ImVec2(208.0f, 30.0f);
}

void ParticleEmitter::EditLayout() {

	ImGui::Begin("ParticleEmitter_EditLayout");

	ImGui::DragFloat2("leftChildSize", &leftChildSize_.x, 0.5f);
	ImGui::DragFloat2("rightChildSize", &rightChildSize_.x, 0.5f);
	ImGui::DragFloat2("addButtonSize", &addButtonSize_.x, 0.5f);
	ImGui::DragFloat2("dropSize", &dropSize_.x, 0.5f);

	ImGui::End();
}

void ParticleEmitter::ImGui() {

	EditLayout();

	// layout
	ImGui::BeginGroup();

	// emitterの名前を表示
	ImGui::SeparatorText(("EmitterName: " + name_).c_str());

	// particle追加処理
	ImGui::BeginChild("AddChild##ParticleEmitter", leftChildSize_, true);
	ImGui::SeparatorText("Add Particle");

	AddParticle();
	ImGui::EndChild();

	// 横並びにする
	ImGui::SameLine();

	// particle選択処理
	ImGui::BeginChild("SelectChild##ParticleEmitter", rightChildSize_, true);
	ImGui::SeparatorText("Select Particle");

	SelectParticle();
	ImGui::EndChild();

	// particleの値操作処理
	ImGui::BeginChild("EditChild##ParticleEmitter");
	ImGui::SeparatorText("Edit Particle");

	EditParticle();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void ParticleEmitter::AddParticle() {

	ImGui::PushID("AddParticlerRow");

	if (ImGui::InputTextWithHint("##ParticleName", "ParticleName",
		addParticleNameInputText_.nameBuffer, IM_ARRAYSIZE(addParticleNameInputText_.nameBuffer))) {

		// 名前を保持
		addParticleNameInputText_.name = addParticleNameInputText_.nameBuffer;
	}

	// ドロップ処理
	if (addModelName_.empty()) {

		// modelをドラッグアンドドロップで取得
		ImGui::Button("Drop model", dropSize_);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

				auto* payloadModelData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
				// model以外は受け付けない
				if (payloadModelData->type == AssetEditor::PendingType::Model) {

					// modelの名前を保存
					addModelName_ = payloadModelData->name;
				}
			}
			ImGui::EndDragDropTarget();
		}
	} else {

		if (ImGui::Button("Reset", ImVec2(addButtonSize_.x / 2.0f, addButtonSize_.y))) {

			// modelの名前をリセットする
			addModelName_.clear();
		}
		ImGui::SameLine();

		// model名を表示
		ImGui::Text((":" + addModelName_).c_str());
	}

	if (ImGui::Button("Add", addButtonSize_)) {

		// 入力がないときは追加不可
		if (!addParticleNameInputText_.name.empty() &&
			!addModelName_.empty()) {

			// particle追加
			particleGroups_.emplace_back();

			// parameterを初期化
			particleGroups_.back().parameter.Init(
				addParticleNameInputText_.name,
				addModelName_, asset_);
			addParticleNameInputText_.Reset();
		}
	}

	ImGui::PopID();
}

void ParticleEmitter::SelectParticle() {

	if (particleGroups_.empty()) {
		ImGui::TextDisabled("Particle empty...");
		return;
	}

	int selectIndex = 0;
	for (int i = 0; i < static_cast<int>(particleGroups_.size()); ++i) {

		const bool selected = (selectIndex == i);
		if (ImGui::Selectable(particleGroups_[i].parameter.GetParticleName().c_str(), selected)) {

			// indexで名前を選択し設定
			selectIndex = i;
			currentSelectIndex_ = i;
		}
		if (selected) {
			ImGui::SetItemDefaultFocus();
		}
	}
}

void ParticleEmitter::EditParticle() {

	// 選択されていないときは何もしない
	if (!currentSelectIndex_.has_value()) {
		return;
	}

	// parameter操作
	particleGroups_[currentSelectIndex_.value()].parameter.ImGui();
}

void ParticleEmitter::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}