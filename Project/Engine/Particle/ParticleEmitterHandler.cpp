#include "ParticleEmitterHandler.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Engine/Asset/AssetEditor.h>
#include <Lib/Adapter/JsonAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	ParticleEmitterHandler classMethods
//============================================================================

void ParticleEmitterHandler::Init() {

	// layout
	leftChildSize_ = ImVec2(320.0f, 150.0f);
	rightChildSize_ = ImVec2(384.0f, 150.0f);
	addButtonSize_ = ImVec2(208.0f, 30.0f);
}

void ParticleEmitterHandler::EditLayout() {

	ImGui::Begin("ParticleEmitterHandler_EditLayout");

	ImGui::DragFloat2("leftChildSize", &leftChildSize_.x, 0.5f);
	ImGui::DragFloat2("rightChildSize", &rightChildSize_.x, 0.5f);
	ImGui::DragFloat2("addButtonSize", &addButtonSize_.x, 0.5f);

	ImGui::End();
}

void ParticleEmitterHandler::ImGui([[maybe_unused]] float itemWidth) {

	EditLayout();

	// layout
	ImGui::BeginGroup();

	// 追加処理
	ImGui::BeginChild("AddChild##ParticleEmitterHandler", leftChildSize_, true);
	ImGui::SeparatorText("Add Emitter");

	AddEmitter();
	LoadEmitter();
	ImGui::EndChild();

	// 横並びにする
	ImGui::SameLine();

	// 選択処理
	ImGui::BeginChild("SelectChild##ParticleEmitterHandler", rightChildSize_, true);
	ImGui::SeparatorText("Select Emitter");

	SelectEmitter();
	ImGui::EndChild();

	ImGui::EndGroup();
}

void ParticleEmitterHandler::ClearNotification() {

	// falseにする
	addEmitter_ = false;
	loadEmitterData_ = std::nullopt;
}

void ParticleEmitterHandler::AddEmitter() {

	ImGui::PushID("AddEmitterRow");

	if (ImGui::InputTextWithHint("##EmitterName", "EmitterName",
		addInputText_.nameBuffer, IM_ARRAYSIZE(addInputText_.nameBuffer))) {

		// 名前を保持
		addInputText_.name = addInputText_.nameBuffer;
	}

	if (ImGui::Button("Add", addButtonSize_)) {

		// 入力がないときは追加不可
		if (!addEmitter_ && !addInputText_.name.empty()) {

			// emitterの名前追加
			emitterNames_.emplace_back(addInputText_.name);
			addInputText_.Reset();
			// systemへ通知する
			addEmitter_ = true;
		}
	}

	ImGui::PopID();
}

void ParticleEmitterHandler::LoadEmitter() {

	ImGui::PushID("LoadEmitterRow");

	ImGui::Button("Load", addButtonSize_);
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadDataId = ImGui::AcceptDragDropPayload(AssetEditor::kDragPayloadId)) {

			auto* payloadJsonData = static_cast<AssetEditor::DragPayload*>(payloadDataId->Data);
			// .json以外は受け付けない
			if (payloadJsonData->type == AssetEditor::PendingType::None) {

				// 読み込み処理
				Json data;
				std::string loadName = "ParticleEmitter/" + std::string(payloadJsonData->name) + ".json";
				if (JsonAdapter::LoadCheck(loadName, data)) {

					// typeがemitterじゃなければ作成できない
					if (data.contains("FileType")) {
						if (data["FileType"] == "ParticleEmitter") {

							// Emitterなので作成する
							// fileの名前をemitterの名前とし、systemへ通知する
							// emitterの名前追加
							emitterNames_.emplace_back(payloadJsonData->name);
							// systemへ通知する
							addEmitter_ = true;
							loadEmitterData_ = data;
						}
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::PopID();
}

void ParticleEmitterHandler::SelectEmitter() {

	if (emitterNames_.empty()) {
		ImGui::TextDisabled("Emitter empty...");
		return;
	}

	for (int i = 0; i < static_cast<int>(emitterNames_.size()); ++i) {

		const bool selected = (currentSelectIndex_ == i);
		if (ImGui::Selectable(emitterNames_[i].c_str(), selected)) {

			// indexで名前を選択し設定
			currentSelectIndex_ = i;
			selectEmitterName_ = emitterNames_[i];
		}
		if (selected) {
			ImGui::SetItemDefaultFocus();
		}
	}
}

void ParticleEmitterHandler::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}