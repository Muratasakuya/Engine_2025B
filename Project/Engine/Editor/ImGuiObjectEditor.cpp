#include "ImGuiObjectEditor.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Object/System/Systems/TagSystem.h>
#include <Engine/Object/Base/Interface/IGameObject.h>
#include <Lib/MathUtils/Algorithm.h>

// data
#include <Engine/Object/Data/Transform.h>
#include <Engine/Object/Data/Material.h>
#include <Engine/Object/Data/Animation.h>
#include <Engine/Object/Data/ObjectTag.h>
#include <Engine/Object/Data/Sprite.h>
#include <Engine/Object/Data/Skybox.h>

// imgui
#include <imgui.h>

//============================================================================
//	ImGuiObjectEditor classMethods
//============================================================================

ImGuiObjectEditor* ImGuiObjectEditor::instance_ = nullptr;

ImGuiObjectEditor* ImGuiObjectEditor::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ImGuiObjectEditor();
	}
	return instance_;
}

void ImGuiObjectEditor::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ImGuiObjectEditor::Init() {

	ObjectManager_ = nullptr;
	ObjectManager_ = ObjectManager::GetInstance();
	tagSystem_ = nullptr;
	tagSystem_ = ObjectManager_->GetSystem<TagSystem>();
}

void ImGuiObjectEditor::SelectObject() {

	CreateGroup();
	SelectGroupedObject();
}

bool ImGuiObjectEditor::Is3D(uint32_t object) const {

	return ObjectManager_->GetData<Transform3D>(object) != nullptr ||
		ObjectManager_->GetData<Skybox>(object) != nullptr;
}

bool ImGuiObjectEditor::Is2D(uint32_t object) const {

	return ObjectManager_->GetData<Transform2D>(object) != nullptr;
}

void ImGuiObjectEditor::DrawSelectable(uint32_t object, const std::string& name) {

	if (Is3D(object)) {
		bool selected = (selected3D_ == object);
		std::string label = name + "##3D" + std::to_string(object);
		if (ImGui::Selectable(label.c_str(), selected)) {
			selected3D_ = object;
			selected2D_.reset();
		}
	}

	if (Is2D(object)) {
		bool selected = (selected2D_ == object);
		std::string label = name + "##2D" + std::to_string(object);
		if (ImGui::Selectable(label.c_str(), selected)) {

			selected2D_ = object;
			selected3D_.reset();
		}
	}
}

void ImGuiObjectEditor::CreateGroup() {

	groups_ = tagSystem_->Groups();
}

void ImGuiObjectEditor::SelectGroupedObject() {

	for (auto& [group, ids] : groups_) {
		if (group.empty()) continue;

		if (ImGui::TreeNode(group.c_str())) {
			for (uint32_t id : ids) {

				const std::string& name = tagSystem_->Tags().at(id)->name;
				DrawSelectable(id, name);
			}
			ImGui::TreePop();
		}
	}
}

void ImGuiObjectEditor::EditObject() {

	// 各objectの操作
	EditObjects();
	EditObject2D();
}

void ImGuiObjectEditor::Reset() {

	selected3D_.reset();
	selected2D_.reset();
}

void ImGuiObjectEditor::Registerobject(uint32_t id, IGameObject* object) {

	objectsMap_[id] = object;
}

void ImGuiObjectEditor::EditObjects() {

	if (!selected3D_) return;
	uint32_t id = selected3D_.value();

	if (Algorithm::Find(objectsMap_, id)) {

		objectsMap_[id].value()->ImGui();
		return;
	}

	if (ImGui::BeginTabBar("Obj3DTab")) {

		const auto* tag = tagSystem_->Tags().at(id);
		// skyboxの時と他のオブジェクトで分岐
		if (tag->name == "skybox") {

			if (ImGui::BeginTabItem("Info")) {

				ObjectsInformation();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Skybox")) {

				EditSkybox();
				ImGui::EndTabItem();
			}
		} else {
			if (ImGui::BeginTabItem("Info")) {

				ObjectsInformation();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Transform")) {

				ObjectsTransform();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Material")) {

				ObjectsMaterial();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void ImGuiObjectEditor::ObjectsInformation() {

	uint32_t id = *selected3D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("objectId: %u", id);

	if (ImGui::Button("Remove")) {

		ObjectManager_->Destroy(id);
		selected3D_.reset();
	}
}

void ImGuiObjectEditor::ObjectsTransform() {

	auto* transform = ObjectManager_->GetData<Transform3D>(*selected3D_);
	transform->ImGui(itemWidth_);
}

void ImGuiObjectEditor::ObjectsMaterial() {

	auto* matsPtr = ObjectManager_->GetData<Material, true>(*selected3D_);
	auto& materials = *matsPtr;

	ImGui::PushItemWidth(itemWidth_);
	if (ImGui::BeginCombo("SelectMaterial",
		("Material " + std::to_string(selectedMaterialIndex_)).c_str())) {
		for (int i = 0; i < static_cast<int>(materials.size()); ++i) {

			bool selected = (selectedMaterialIndex_ == i);
			std::string label = "Material " + std::to_string(i);
			if (ImGui::Selectable(label.c_str(), selected)) {

				selectedMaterialIndex_ = i;
			}
			if (selected) {

				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	selectedMaterialIndex_ = std::clamp(selectedMaterialIndex_, 0, static_cast<int>(materials.size() - 1));
	if (!materials.empty()) {

		materials[selectedMaterialIndex_].ImGui(itemWidth_);
	}
}

void ImGuiObjectEditor::EditSkybox() {

	auto* skybox = ObjectManager_->GetData<Skybox>(*selected3D_);
	skybox->ImGui(itemWidth_);
}

void ImGuiObjectEditor::EditObject2D() {

	if (!selected2D_) return;
	uint32_t id = selected2D_.value();

	if (ImGui::BeginTabBar("Obj2DTab")) {

		if (ImGui::BeginTabItem("Info")) {

			Object2DInformation();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Sprite")) {

			Object2DSprite();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Transform")) {

			Object2DTransform();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			Object2DMaterial();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Derived")) {
			if (Algorithm::Find(objectsMap_, id)) {

				objectsMap_[id].value()->DerivedImGui();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void ImGuiObjectEditor::Object2DInformation() {

	uint32_t id = *selected2D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("objectId: %u", id);

	if (ImGui::Button("Remove")) {

		ObjectManager_->Destroy(id);
		selected2D_.reset();
	}
}

void ImGuiObjectEditor::Object2DSprite() {

	auto* sprite = ObjectManager_->GetData<Sprite>(*selected2D_);
	sprite->ImGui(itemWidth_);
}

void ImGuiObjectEditor::Object2DTransform() {

	auto* transform = ObjectManager_->GetData<Transform2D>(*selected2D_);
	transform->ImGui(itemWidth_);
}

void ImGuiObjectEditor::Object2DMaterial() {

	auto* material = ObjectManager_->GetData<SpriteMaterial>(*selected2D_);
	material->ImGui(itemWidth_);
}