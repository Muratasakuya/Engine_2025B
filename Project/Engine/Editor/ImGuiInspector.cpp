#include "ImGuiInspector.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/System/Systems/TagSystem.h>
#include <Lib/MathUtils/Algorithm.h>

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>

// imgui
#include <imgui.h>

//============================================================================
//	ImGuiInspector classMethods
//============================================================================

ImGuiInspector* ImGuiInspector::instance_ = nullptr;

ImGuiInspector* ImGuiInspector::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ImGuiInspector();
	}
	return instance_;
}

void ImGuiInspector::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ImGuiInspector::Init() {

	ecsManager_ = nullptr;
	ecsManager_ = ECSManager::GetInstance();
	tagSystem_ = nullptr;
	tagSystem_ = ecsManager_->GetSystem<TagSystem>();
}

void ImGuiInspector::SelectObject() {

	CreateGroup();
	SelectGroupedObject();
	SelectUnGroupedObject();
}

void ImGuiInspector::CreateGroup() {

	groups_ = tagSystem_->Groups();
}

void ImGuiInspector::SelectGroupedObject() {

	for (auto& [group, ids] : groups_) {
		if (group.empty()) {
			continue;
		}

		if (ImGui::TreeNode(group.c_str())) {
			for (uint32_t id : ids) {

				const std::string& name = tagSystem_->Tags().at(id)->name;
				bool selected = (selected3D_ == id);
				if (ImGui::Selectable(name.c_str(), selected)) {

					selected3D_ = id;
				}
			}
			ImGui::TreePop();
		}
	}
}

void ImGuiInspector::SelectUnGroupedObject() {

	if (groups_.count("") == 0) return;
	for (uint32_t id : groups_[""]) {

		const std::string& name = tagSystem_->Tags().at(id)->name;
		bool selected = (selected3D_ == id);
		if (ImGui::Selectable(name.c_str(), selected)) {

			selected3D_ = id;
		}
	}
}

void ImGuiInspector::EditObject() {

	// 各objectの操作
	EditObject3D();
}

void ImGuiInspector::Reset() {

	selected3D_.reset();
}

void ImGuiInspector::SetImGuiFunc(uint32_t entity, std::function<void()> func) {

	individualUI_[entity] = std::move(func);
}

void ImGuiInspector::EditObject3D() {

	if (!selected3D_) return;

	if (ImGui::BeginTabBar("Obj3DTab")) {

		if (ImGui::BeginTabItem("Info")) {

			Object3DInformation();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Transform")) {

			Object3DTransform();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			Object3DMaterial();
			ImGui::EndTabItem();
		}

		if (selected3D_.has_value()) {
			if (ImGui::BeginTabItem("Individual")) {
				if (!individualUI_.empty()) {

					individualUI_.at(*selected3D_)();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void ImGuiInspector::Object3DInformation() {

	uint32_t id = *selected3D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("entityId: %u", id);

	if (ImGui::Button("Remove")) {

		ecsManager_->Destroy(id);
		selected3D_.reset();
	}
}

void ImGuiInspector::Object3DTransform() {

	auto* transform = ecsManager_->GetComponent<Transform3DComponent>(*selected3D_);
	transform->ImGui(itemWidth_);
}

void ImGuiInspector::Object3DMaterial() {

	auto* matsPtr = ecsManager_->GetComponent<MaterialComponent, true>(*selected3D_);
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