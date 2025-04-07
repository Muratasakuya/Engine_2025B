#include "Object3DEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>

//============================================================================
//	Object3DEditor classMethods
//============================================================================

void Object3DEditor::Init(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;
}

void Object3DEditor::ImGui() {

	// editorの関数群

	// objectの追加
	AddObject();
}

void Object3DEditor::AddObject() {

	// 現在loadされているmodel
	const std::vector<std::string> modelKeys = asset_->GetModelKeys();

	// modelの名前の選択
	if (ImGui::BeginCombo("Model", modelKeys[addSelectedObjectIndex_].c_str())) {
		for (int index = 0; index < modelKeys.size(); ++index) {

			const bool isSelected = (addSelectedObjectIndex_ == index);
			if (ImGui::Selectable(modelKeys[index].c_str(), isSelected)) {

				// 追加する予定のmodelを設定
				addSelectedObjectIndex_ = index;
				addModelName_ = modelKeys[index];
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 追加するobjectの名前
	addObjectName_.InputText("ObjectName##Add");
	// 追加するgroupの名前
	addGroupName_.InputText("GroupName##Add");
}

void Object3DEditor::InputTextValue::InputText(const std::string& label) {

	// 名前の入力
	if (ImGui::InputText(label.c_str(),
		nameBuffer,
		IM_ARRAYSIZE(nameBuffer))) {

		name = nameBuffer;
	}
}