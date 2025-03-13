#include "ImGuiComponentManager.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Component/Manager/ComponentManager.h>
#include <Lib/MathUtils/Algorithm.h>

// imgui
#include <imgui.h>

//============================================================================
//	ImGuiComponentManager classMethods
//============================================================================

void ImGuiComponentManager::Init(EntityManager* entityManager, Transform3DManager* transform3DManager,
	MaterialManager* materialManager, ModelComponentManager* modelComponentManager,
	RenderObjectManager* renderObjectManager) {

	entityManager_ = nullptr;
	entityManager_ = entityManager;

	transform3DManager_ = nullptr;
	transform3DManager_ = transform3DManager;

	materialManager_ = nullptr;
	materialManager_ = materialManager;

	modelComponentManager_ = nullptr;
	modelComponentManager_ = modelComponentManager;

	renderObjectManager_ = nullptr;
	renderObjectManager_ = renderObjectManager;
}

void ImGuiComponentManager::SelectObject3D() {

	ImGui::SeparatorText("Object3D");

	// 名前の基本部分と数値部分を取得する関数
	auto GetBaseNameAndNumber = [](const std::string& name) -> std::pair<std::string, int> {
		size_t index = name.find_last_not_of("0123456789");
		if (index != std::string::npos && index + 1 < name.size()) {

			std::string baseName = name.substr(0, index + 1);
			int number = std::stoi(name.substr(index + 1));
			return { baseName, number };
		}
		return { name, 0 };
		};

	if (entityManager_->GetNames().size() != object3D_.lastCount_) {

		object3D_.grouped_.clear();
		for (const auto& [id, name] : entityManager_->GetNames()) {
			auto [baseName, number] = GetBaseNameAndNumber(name);
			object3D_.grouped_[baseName].emplace_back(number, id);
		}

		object3D_.lastCount_ = entityManager_->GetNames().size();
	}

	// objectNameの表示
	for (auto& [baseName, objects] : object3D_.grouped_) {
		if (objects.size() == 1) {
			if (ImGui::Selectable(baseName.c_str(), object3D_.selectedId_ == objects[0].second)) {
				object3D_.selectedId_ = objects[0].second;
			}
		} else {
			if (ImGui::TreeNode(baseName.c_str())) {
				for (const auto& [number, id] : objects) {

					std::string label = baseName + std::to_string(number);
					if (ImGui::Selectable(label.c_str(), object3D_.selectedId_ == id)) {

						// idの更新処理
						object3D_.selectedId_ = id;
						selectedMaterialIndex_ = 0;
					}
				}
				ImGui::TreePop();
			}
		}
	}
}

void ImGuiComponentManager::EditObject3D() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(transform3DManager_->GetComponent(*object3D_.selectedId_), "does not exist object3D:transform");
	ASSERT(!materialManager_->GetComponentList(*object3D_.selectedId_).empty(), "does not exist object3D:material");

	Object3DInformation();

	ImGui::Separator();

	if (ImGui::BeginTabBar("Object3DTabs")) {

		if (ImGui::BeginTabItem("Rendering")) {

			Object3DRenderingData();
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

		if (ImGui::BeginTabItem("Individual")) {

			if (Algorithm::Find(object3D_.imguiFunc_, *object3D_.selectedId_)) {

				EntityID id = *object3D_.selectedId_;
				object3D_.imguiFunc_.at(id)();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImGuiComponentManager::SetImGuiFunc(EntityID entityId, std::function<void()> func) {

	object3D_.imguiFunc_[entityId] = func;
}

void ImGuiComponentManager::Object3DInformation() {

	ImGui::Text("name: %s", entityManager_->GetNames().at(*object3D_.selectedId_).c_str());
	ImGui::Text("entityId: %d", *object3D_.selectedId_);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveObject3D(*object3D_.selectedId_);
		object3D_.selectedId_ = std::nullopt;
	}
}

void ImGuiComponentManager::Object3DRenderingData() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ModelComponent* model = modelComponentManager_->GetComponent(*object3D_.selectedId_);

	model->ImGui(itemWidth_, renderObjectManager_);
}

void ImGuiComponentManager::Object3DTransform() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	Transform3DComponent* transform = transform3DManager_->GetComponent(*object3D_.selectedId_);

	transform->ImGui(itemWidth_);
}

void ImGuiComponentManager::Object3DMaterial() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	std::vector<Material*> materials = materialManager_->GetComponentList(*object3D_.selectedId_);

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

		selectedMaterialIndex_ = std::clamp(selectedMaterialIndex_, 0, static_cast<int>(materials.size() - 1));
		materials[selectedMaterialIndex_]->ImGui(itemWidth_);
	}
}