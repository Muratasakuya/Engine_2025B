#include "PrimitiveRegistry.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core\Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	PrimitiveRegistry classMethods
//============================================================================

void PrimitiveRegistry::Init(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	primitiveMeshData_ = std::make_unique<PrimitiveMeshData>();
	primitiveMeshData_->Create(asset);

	selectPrimitiveIndex_ = std::nullopt;
}

void PrimitiveRegistry::EditLayout() {

	// createParameter
	ImGui::DragFloat("createParameterWidth", &parameterWidth_, 0.5f);
	ImGui::DragFloat("upLayoutHeight", &upLayoutHeight_, 0.5f);
}

void PrimitiveRegistry::RegistryPrimitiveMesh() {

	// 範囲開始
	ImGui::BeginChild("leftLayout",
		ImVec2(0.0f, upLayoutHeight_),
		ImGuiChildFlags_Border |
		ImGuiChildFlags_ResizeX);

	ImGui::SeparatorText("Create");

	// 横幅設定
	ImGui::PushItemWidth(parameterWidth_);

	// primitiveの選択
	if (ImGui::BeginCombo("Primitive", primitiveMeshData_->GetNames()[createSelectedPrimitiveIndex_].c_str())) {
		for (int index = 0; index < primitiveMeshData_->GetNames().size(); ++index) {

			const bool isSelected = (createSelectedPrimitiveIndex_ == index);
			if (ImGui::Selectable(primitiveMeshData_->GetNames()[index].c_str(), isSelected)) {

				// 追加する予定のmodelを設定
				createSelectedPrimitiveIndex_ = index;
				createPrimitiveName_ = primitiveMeshData_->GetNames()[index];
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// textureの選択
	const std::vector<std::string> textureKeys = asset_->GetTextureKeys();
	if (ImGui::BeginCombo("Texture", textureKeys[createSelectedTextureIndex_].c_str())) {
		for (int index = 0; index < textureKeys.size(); ++index) {

			const bool isSelected = (createSelectedTextureIndex_ == index);
			if (ImGui::Selectable(textureKeys[index].c_str(), isSelected)) {

				// 追加する予定のtexture設定
				createSelectedTextureIndex_ = index;
				createTextureName_ = textureKeys[index];
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 保存するときのobjectの名前
	createObjectName_.InputText("ObjectName##Create");

	if (ImGui::Button("Create Primitive")) {

		// 名前が入力されていなければ追加出来ない
		if (!createPrimitiveName_.empty() ||
			createObjectName_.name.empty()) {

			// 名前の重複確認
			std::string uniqueObjectName = CheckName(createObjectName_.name);

			// primitriveの名前を足す
			uniqueObjectName = uniqueObjectName + ":" + createPrimitiveName_;

			// 追加、作成
			selectPrimitives_[uniqueObjectName] =
				GameObjectHelper::CreateEffect(
					createPrimitiveName_,
					createTextureName_,
					uniqueObjectName);
		}
	}

	ImGui::PopItemWidth();

	// 終了
	ImGui::EndChild();
}

void PrimitiveRegistry::SelectPrimiviveMesh() {

	// 追加と横並びにする
	ImGui::SameLine();

	// 範囲開始
	ImGui::BeginChild("rightLayout",
		ImVec2(0.0f, upLayoutHeight_),
		ImGuiChildFlags_Border);

	ImGui::SeparatorText("Select");

	for (const auto& [objectName, id] : selectPrimitives_) {

		bool isSelected = (selectPrimitiveIndex_.value_or(-1) == static_cast<int>(id));
		if (ImGui::Selectable(objectName.c_str(), isSelected)) {

			// 選択されたindexを設定
			selectPrimitiveIndex_ = id;
		}
		if (isSelected) {

			ImGui::SetItemDefaultFocus();
		}
	}

	ImGui::EndChild();
}

void PrimitiveRegistry::ApplyEditLayoutParameter(const Json& data) {

	parameterWidth_ = JsonAdapter::GetValue<float>(data, "parameterWidth_");
	upLayoutHeight_ = JsonAdapter::GetValue<float>(data, "upLayoutHeight_");
}

void PrimitiveRegistry::SaveEditLayoutParameter(Json& data) {

	data["parameterWidth_"] = parameterWidth_;
	data["upLayoutHeight_"] = upLayoutHeight_;
}

std::string PrimitiveRegistry::CheckName(const std::string& name) {

	int trailingNumber = 0;
	std::string base = SplitBaseNameAndNumber(name, trailingNumber);

	int& count = nameCounts_[base];

	if (trailingNumber > count) {
		count = trailingNumber;
	}

	std::string uniqueName;
	if (count == 0) {

		uniqueName = base;
	} else {

		uniqueName = base + std::to_string(count);
	}

	count++;

	return uniqueName;
}

std::string PrimitiveRegistry::SplitBaseNameAndNumber(const std::string& name, int& number) {

	int idx = static_cast<int>(name.size()) - 1;
	while (idx >= 0 && std::isdigit(name[idx])) {
		idx--;
	}

	int startOfDigits = idx + 1;
	if (startOfDigits < static_cast<int>(name.size())) {

		// 末尾に数字がある場合
		number = std::stoi(name.substr(startOfDigits));
	} else {

		// 末尾に数字が無い場合
		number = 0;
	}

	return name.substr(0, startOfDigits);
}

void PrimitiveRegistry::InputTextValue::InputText(const std::string& label) {

	// 名前の入力
	if (ImGui::InputText(label.c_str(),
		nameBuffer,
		IM_ARRAYSIZE(nameBuffer))) {

		name = nameBuffer;
	}
}

void PrimitiveRegistry::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}