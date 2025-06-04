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
#include <Engine/Core/ECS/Components/SpriteComponent.h>

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
}

bool ImGuiInspector::Is3D(uint32_t entity) const {

	return ecsManager_->GetComponent<Transform3DComponent>(entity) != nullptr;
}

bool ImGuiInspector::Is2D(uint32_t entity) const {

	return ecsManager_->GetComponent<Transform2DComponent>(entity) != nullptr;
}

void ImGuiInspector::DrawSelectable(uint32_t entity, const std::string& name) {

	if (Is3D(entity)) {
		bool selected = (selected3D_ == entity);
		std::string label = name + "##3D" + std::to_string(entity);
		if (ImGui::Selectable(label.c_str(), selected)) {
			selected3D_ = entity;
			selected2D_.reset();
		}
	}

	if (Is2D(entity)) {
		bool selected = (selected2D_ == entity);
		std::string label = name + "##2D" + std::to_string(entity);
		if (ImGui::Selectable(label.c_str(), selected)) {

			selected2D_ = entity;
			selected3D_.reset();
		}
	}
}

void ImGuiInspector::CreateGroup() {

	groups_ = tagSystem_->Groups();
}

void ImGuiInspector::SelectGroupedObject() {

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

void ImGuiInspector::EditObject() {

	// 各objectの操作
	EditObject3D();
	EditObject2D();
}

void ImGuiInspector::Reset() {

	selected3D_.reset();
	selected2D_.reset();
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

void ImGuiInspector::EditObject2D() {

	if (!selected2D_) return;

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

		if (selected2D_.has_value()) {
			if (ImGui::BeginTabItem("Individual")) {
				if (!individualUI_.empty()) {

					individualUI_.at(*selected2D_)();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void ImGuiInspector::Object2DInformation() {

	uint32_t id = *selected2D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("entityId: %u", id);

	if (ImGui::Button("Remove")) {

		ecsManager_->Destroy(id);
		selected2D_.reset();
	}
}

void ImGuiInspector::Object2DSprite() {

	auto* sprite = ecsManager_->GetComponent<SpriteComponent>(*selected2D_);
	sprite->ImGui(itemWidth_);
}

void ImGuiInspector::Object2DTransform() {

	auto* transform = ecsManager_->GetComponent<Transform2DComponent>(*selected2D_);
	transform->ImGui(itemWidth_);
}

void ImGuiInspector::Object2DMaterial() {

	auto* material = ecsManager_->GetComponent<SpriteMaterialComponent>(*selected2D_);
	material->ImGui(itemWidth_);
}