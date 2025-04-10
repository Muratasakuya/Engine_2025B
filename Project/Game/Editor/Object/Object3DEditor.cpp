#include "Object3DEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	Object3DEditor classMethods
//============================================================================

void Object3DEditor::Init(Asset* asset) {

	asset_ = nullptr;
	asset_ = asset;

	// 初期化値
	editLayoutEnable_ = false;
	selectObjectIndex_ = std::nullopt;
	selectedMaterialIndex_ = -1;

	// json適応、設定
	ApplyJson();
}

void Object3DEditor::ImGui() {

	// editorの関数群

	// editor内のlayoutを調整
	EditLayout();

	ImGui::Separator();

	// objectの追加
	AddObject();
	// objectの保存
	SaveObjects();

	// objectの選択
	SelectObject();

	// objectの操作
	EditObject();
}

void Object3DEditor::SaveEditLayoutParameter() {

	Json data;

	data["addParameterWidth_"] = addParameterWidth_;
	data["upLayoutHeight_"] = upLayoutHeight_;
	data["itemWidth_"] = itemWidth_;

	JsonAdapter::Save(baseJsonPath_ + "editorLayoutParameter.json", data);
}

void Object3DEditor::ApplyJson() {

	// editor内のlayoutParameterを設定
	ApplyEditLayoutParameter();
}

void Object3DEditor::ApplyEditLayoutParameter() {

	if (!JsonAdapter::LoadAssert(baseJsonPath_ + "editorLayoutParameter.json")) {
		return;
	}

	Json data = JsonAdapter::Load(baseJsonPath_ + "editorLayoutParameter.json");

	addParameterWidth_ = JsonAdapter::GetValue<float>(data, "addParameterWidth_");
	upLayoutHeight_ = JsonAdapter::GetValue<float>(data, "upLayoutHeight_");
	itemWidth_ = JsonAdapter::GetValue<float>(data, "itemWidth_");
}

void Object3DEditor::SaveJson() {


}

void Object3DEditor::EditLayout() {

	ImGui::Checkbox("editLayout", &editLayoutEnable_);

	if (!editLayoutEnable_) {
		return;
	}

	ImGui::Begin("objectEditorLayout");

	if (ImGui::Button("Save")) {

		SaveEditLayoutParameter();
	}

	// addParameter
	ImGui::DragFloat("addParameterWidth", &addParameterWidth_, 0.5f);
	ImGui::DragFloat("upLayoutHeight", &upLayoutHeight_, 0.5f);
	ImGui::DragFloat("itemWidth", &itemWidth_, 0.5f);

	ImGui::End();
}

void Object3DEditor::AddObject() {

	// 範囲開始
	ImGui::BeginChild("leftLayout",
		ImVec2(0.0f, upLayoutHeight_),
		ImGuiChildFlags_Border |
		ImGuiChildFlags_ResizeX);

	ImGui::SeparatorText("Add");

	// 現在loadされているmodel
	const std::vector<std::string> modelKeys = asset_->GetModelKeys();

	// 横幅設定
	ImGui::PushItemWidth(addParameterWidth_);

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

	if (ImGui::Button("Add Object")) {

		// 名前が入力されていなければ追加出来ない
		if (!addModelName_.empty() &&
			!addObjectName_.name.empty() &&
			!addGroupName_.name.empty()) {

			// 名前の重複確認
			std::string uniqueObjectName = CheckName(addObjectName_.name);

			// 入力された名前で選択リストに追加
			int id = GameObjectHelper::CreateObject3D(
				addModelName_, uniqueObjectName, addGroupName_.name);
			// 追加
			groupedSelectObjects_[addGroupName_.name][uniqueObjectName] = id;

			// 追加されたタイミングで入力をリセット
			addObjectName_.Reset();
			addGroupName_.Reset();
		}
	}
}

void Object3DEditor::SaveObjects() {

	ImGui::SeparatorText("Save");

	// 追加するobjectの名前
	saveFileName_.InputText("FileName##Save");
	// 保存するgroupの名前
	saveGroupName_.InputText("GroupName##Save");

	// json保存
	if (ImGui::Button("Save GroupObjects")) {

		SaveJson();

		// 保存されたタイミングで入力をリセット
		saveFileName_.Reset();
		saveGroupName_.Reset();
	}

	ImGui::PopItemWidth();

	// AddObjectにbeginはある、終了
	ImGui::EndChild();
}

void Object3DEditor::SelectObject() {

	// 追加と横並びにする
	ImGui::SameLine();

	// 範囲開始
	ImGui::BeginChild("rightLayout",
		ImVec2(0.0f, upLayoutHeight_),
		ImGuiChildFlags_Border);

	// グループごとにTree表示
	for (const auto& [groupName, objectMap] : groupedSelectObjects_) {

		// グループノードを表示
		if (ImGui::TreeNode(groupName.c_str())) {
			for (const auto& [objectName, id] : objectMap) {

				bool isSelected = (selectObjectIndex_.value_or(-1) == static_cast<int>(id));
				if (ImGui::Selectable(objectName.c_str(), isSelected)) {

					// 選択されたindexを設定
					selectObjectIndex_ = id;
				}
				if (isSelected) {

					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::TreePop();
		}
	}

	ImGui::EndChild();
}

void Object3DEditor::EditObject() {

	// 範囲開始
	ImGui::BeginChild("bottomLayout",
		ImVec2(), ImGuiChildFlags_Border);

	// objectの削除
	RemoveObject();

	if (ImGui::BeginTabBar("Object3DTabs")) {

		if (ImGui::BeginTabItem("Mesh")) {

			// meshの操作
			EditMesh();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Transform")) {

			// transformの操作
			EditTransform();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			// materialの操作
			EditMaterial();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	// 終了
	ImGui::EndChild();
}

void Object3DEditor::RemoveObject() {

	// 選択されていなければ何もしない
	if (!selectObjectIndex_.has_value()) {
		return;
	}

	ImGui::PushItemWidth(itemWidth_);

	// 選択しているobjectの削除
	if (ImGui::Button("Remove")) {

		GameObjectHelper::RemoveObject3D(selectObjectIndex_.value());

		// 名前のリストから消す
		for (auto groupIt = groupedSelectObjects_.begin(); groupIt != groupedSelectObjects_.end(); ++groupIt) {
			for (auto objIt = groupIt->second.begin(); objIt != groupIt->second.end(); ++objIt) {
				if (objIt->second == *selectObjectIndex_) {

					groupIt->second.erase(objIt);
					break;
				}
			}
			// グループ内が空になったら削除
			if (groupIt->second.empty()) {

				groupedSelectObjects_.erase(groupIt);
				break;
			}
		}
		// 選択解除
		selectObjectIndex_ = std::nullopt;
	}
	ImGui::PopItemWidth();
}

void Object3DEditor::EditMesh() {

	// 今は特に何もない
}

void Object3DEditor::EditTransform() {

	// 選択されていなければ何もしない
	if (!selectObjectIndex_.has_value()) {
		return;
	}

	Transform3DComponent* transform =
		Component::GetComponent<Transform3DComponent>(selectObjectIndex_.value());

	transform->ImGui(itemWidth_);
}

void Object3DEditor::EditMaterial() {

	// 選択されていなければ何もしない
	if (!selectObjectIndex_.has_value()) {
		return;
	}

	std::vector<MaterialComponent*> materials =
		Component::GetComponentList<MaterialComponent>(selectObjectIndex_.value());

	ImGui::PushItemWidth(itemWidth_);
	// Materialの選択
	if (ImGui::BeginCombo("SelectMaterial", ("Material " + std::to_string(selectedMaterialIndex_)).c_str())) {
		for (size_t i = 0; i < materials.size(); ++i) {

			bool isSelected = (selectedMaterialIndex_ == static_cast<int>(i));
			std::string label = "Material " + std::to_string(i);
			if (ImGui::Selectable(label.c_str(), isSelected)) {

				selectedMaterialIndex_ = static_cast<int>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	// 選択されたMaterialを表示
	if (!materials.empty()) {

		// materialのサイズに制限
		selectedMaterialIndex_ = std::clamp(selectedMaterialIndex_, 0, static_cast<int>(materials.size() - 1));
		materials[selectedMaterialIndex_]->ImGui(itemWidth_);
	}
}

std::string Object3DEditor::CheckName(const std::string& name) {

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

std::string Object3DEditor::SplitBaseNameAndNumber(const std::string& name, int& number) {

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

void Object3DEditor::InputTextValue::InputText(const std::string& label) {

	// 名前の入力
	if (ImGui::InputText(label.c_str(),
		nameBuffer,
		IM_ARRAYSIZE(nameBuffer))) {

		name = nameBuffer;
	}
}

void Object3DEditor::InputTextValue::Reset() {

	// 入力をリセット
	nameBuffer[0] = '\0';
	name.clear();
}