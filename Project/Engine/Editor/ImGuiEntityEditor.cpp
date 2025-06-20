#include "ImGuiEntityEditor.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/System/Systems/TagSystem.h>
#include <Engine/Entity/Interface/IGameEntity.h>
#include <Lib/MathUtils/Algorithm.h>

// components
#include <Engine/Core/ECS/Components/TransformComponent.h>
#include <Engine/Core/ECS/Components/MaterialComponent.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Core/ECS/Components/TagComponent.h>
#include <Engine/Core/ECS/Components/SpriteComponent.h>
#include <Engine/Core/ECS/Components/SkyboxComponent.h>

// imgui
#include <imgui.h>

//============================================================================
//	ImGuiEntityEditor classMethods
//============================================================================

ImGuiEntityEditor* ImGuiEntityEditor::instance_ = nullptr;

ImGuiEntityEditor* ImGuiEntityEditor::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new ImGuiEntityEditor();
	}
	return instance_;
}

void ImGuiEntityEditor::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

void ImGuiEntityEditor::Init() {

	ecsManager_ = nullptr;
	ecsManager_ = ECSManager::GetInstance();
	tagSystem_ = nullptr;
	tagSystem_ = ecsManager_->GetSystem<TagSystem>();
}

void ImGuiEntityEditor::SelectObject() {

	CreateGroup();
	SelectGroupedObject();
}

bool ImGuiEntityEditor::Is3D(uint32_t entity) const {

	return ecsManager_->GetComponent<Transform3DComponent>(entity) != nullptr ||
		ecsManager_->GetComponent<SkyboxComponent>(entity) != nullptr;
}

bool ImGuiEntityEditor::Is2D(uint32_t entity) const {

	return ecsManager_->GetComponent<Transform2DComponent>(entity) != nullptr;
}

void ImGuiEntityEditor::DrawSelectable(uint32_t entity, const std::string& name) {

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

void ImGuiEntityEditor::CreateGroup() {

	groups_ = tagSystem_->Groups();
}

void ImGuiEntityEditor::SelectGroupedObject() {

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

void ImGuiEntityEditor::EditObject() {

	// 各objectの操作
	EditObject3D();
	EditObject2D();
}

void ImGuiEntityEditor::Reset() {

	selected3D_.reset();
	selected2D_.reset();
}

void ImGuiEntityEditor::RegisterEntity(uint32_t id, IGameEntity* entity) {

	entitiesMap_[id] = entity;
}

void ImGuiEntityEditor::EditObject3D() {

	if (!selected3D_) return;
	uint32_t id = selected3D_.value();

	if (Algorithm::Find(entitiesMap_, id)) {

		entitiesMap_[id].value()->ImGui();
		return;
	}

	if (ImGui::BeginTabBar("Obj3DTab")) {

		const auto* tag = tagSystem_->Tags().at(id);
		// skyboxの時と他のオブジェクトで分岐
		if (tag->name == "skybox") {

			if (ImGui::BeginTabItem("Info")) {

				Object3DInformation();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Skybox")) {

				EditSkybox();
				ImGui::EndTabItem();
			}
		} else {
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
		}
		ImGui::EndTabBar();
	}
}

void ImGuiEntityEditor::Object3DInformation() {

	uint32_t id = *selected3D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("entityId: %u", id);

	if (ImGui::Button("Remove")) {

		ecsManager_->Destroy(id);
		selected3D_.reset();
	}
}

void ImGuiEntityEditor::Object3DTransform() {

	auto* transform = ecsManager_->GetComponent<Transform3DComponent>(*selected3D_);
	transform->ImGui(itemWidth_);
}

void ImGuiEntityEditor::Object3DMaterial() {

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

void ImGuiEntityEditor::EditSkybox() {

	auto* skybox = ecsManager_->GetComponent<SkyboxComponent>(*selected3D_);
	skybox->ImGui(itemWidth_);
}

void ImGuiEntityEditor::EditObject2D() {

	if (!selected2D_) return;
	uint32_t id = selected2D_.value();

	if (Algorithm::Find(entitiesMap_, id)) {

		entitiesMap_[id].value()->ImGui();
		return;
	}

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
		ImGui::EndTabBar();
	}
}

void ImGuiEntityEditor::Object2DInformation() {

	uint32_t id = *selected2D_;
	const auto* tag = tagSystem_->Tags().at(id);

	ImGui::Text("name: %s", tag->name.c_str());
	ImGui::Text("entityId: %u", id);

	if (ImGui::Button("Remove")) {

		ecsManager_->Destroy(id);
		selected2D_.reset();
	}
}

void ImGuiEntityEditor::Object2DSprite() {

	auto* sprite = ecsManager_->GetComponent<SpriteComponent>(*selected2D_);
	sprite->ImGui(itemWidth_);
}

void ImGuiEntityEditor::Object2DTransform() {

	auto* transform = ecsManager_->GetComponent<Transform2DComponent>(*selected2D_);
	transform->ImGui(itemWidth_);
}

void ImGuiEntityEditor::Object2DMaterial() {

	auto* material = ecsManager_->GetComponent<SpriteMaterialComponent>(*selected2D_);
	material->ImGui(itemWidth_);
}