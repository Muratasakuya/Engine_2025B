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

void ImGuiComponentManager::Init(
	// 3D
	EntityManager* entity3DManager, Transform3DManager* transform3DManager,
	MaterialManager* materialManager, ModelComponentManager* modelComponentManager,
	// 2D
	EntityManager* entity2DManager, Transform2DManager* transform2DManager,
	SpriteMaterialManager* spriteMaterialManager, SpriteComponentManager* spriteComponentManager) {

	entity3DManager_ = nullptr;
	entity3DManager_ = entity3DManager;

	transform3DManager_ = nullptr;
	transform3DManager_ = transform3DManager;

	materialManager_ = nullptr;
	materialManager_ = materialManager;

	modelComponentManager_ = nullptr;
	modelComponentManager_ = modelComponentManager;

	entity2DManager_ = nullptr;
	entity2DManager_ = entity2DManager;

	transform2DManager_ = nullptr;
	transform2DManager_ = transform2DManager;

	spriteMaterialManager_ = nullptr;
	spriteMaterialManager_ = spriteMaterialManager;

	spriteComponentManager_ = nullptr;
	spriteComponentManager_ = spriteComponentManager;
}

void ImGuiComponentManager::SelectObject() {

	// groupの作成
	CreateGroup();
	// objectの選択
	SelectGroupedObject();
	SelectUnGroupedObject();
}

void ImGuiComponentManager::CreateGroup() {

	// グループをクリア
	groupedEntities_.clear();

	// entityGroupの作成
	// 3D
	for (const EntityID id : entity3DManager_->GetIndexToEntity()) {

		const auto& entity = entity3DManager_->GetNames()[entity3DManager_->GetIndex(id)];
		std::string group = entity.groupName.value_or("");
		groupedEntities_[group].push_back(EntityReference(EntityType::Object3D, id));
	}

	// 2D
	for (const EntityID id : entity2DManager_->GetIndexToEntity()) {

		const auto& entity = entity2DManager_->GetNames()[entity2DManager_->GetIndex(id)];
		std::string group = entity.groupName.value_or("");
		groupedEntities_[group].push_back(EntityReference(EntityType::Object2D, id));
	}
}

void ImGuiComponentManager::SelectGroupedObject() {

	// group:TreeNode:Selectable
	for (const auto& [groupName, entities] : groupedEntities_) {
		if (!groupName.empty()) {
			if (ImGui::TreeNode(groupName.c_str())) {

				// Object3D表示
				bool has3D = std::any_of(entities.begin(), entities.end(), [](const auto& ref) {
					return ref.type == EntityType::Object3D;
					});
				if (has3D) {
					for (const auto& ref : entities) {
						if (ref.type != EntityType::Object3D) continue;

						const auto& name = entity3DManager_->GetNames().at(entity3DManager_->GetIndex(ref.id)).name;
						bool selected = (object3D_.selectedId_ == ref.id);

						if (ImGui::Selectable(name.c_str(), selected)) {

							object3D_.selectedId_ = ref.id;
							object2D_.selectedId_ = std::nullopt;
						}
					}
				}

				// Object2D表示
				bool has2D = std::any_of(entities.begin(), entities.end(), [](const auto& ref) {
					return ref.type == EntityType::Object2D;
					});
				if (has2D) {
					for (const auto& ref : entities) {
						if (ref.type != EntityType::Object2D) continue;

						const auto& name = entity2DManager_->GetNames().at(entity2DManager_->GetIndex(ref.id)).name;
						bool selected = (object2D_.selectedId_ == ref.id);

						if (ImGui::Selectable(name.c_str(), selected)) {

							object2D_.selectedId_ = ref.id;
							object3D_.selectedId_ = std::nullopt;
						}
					}
				}

				ImGui::TreePop();
			}
		}
	}
}

void ImGuiComponentManager::SelectUnGroupedObject() {

	// Selectable
	if (groupedEntities_.count("") > 0) {

		const auto& ungroupedEntities = groupedEntities_[""];

		// Object3D表示
		bool has3D = std::any_of(ungroupedEntities.begin(), ungroupedEntities.end(), [](const auto& ref) {
			return ref.type == EntityType::Object3D;
			});
		if (has3D) {
			for (const auto& ref : ungroupedEntities) {
				if (ref.type != EntityType::Object3D) continue;

				const auto& name = entity3DManager_->GetNames().at(entity3DManager_->GetIndex(ref.id)).name;
				bool selected = (object3D_.selectedId_ == ref.id);

				if (ImGui::Selectable(name.c_str(), selected)) {

					object3D_.selectedId_ = ref.id;
					object2D_.selectedId_ = std::nullopt;
				}
			}
		}

		// Object2D表示
		bool has2D = std::any_of(ungroupedEntities.begin(), ungroupedEntities.end(), [](const auto& ref) {
			return ref.type == EntityType::Object2D;
			});
		if (has2D) {
			for (const auto& ref : ungroupedEntities) {
				if (ref.type != EntityType::Object2D) continue;

				const auto& name = entity2DManager_->GetNames().at(entity2DManager_->GetIndex(ref.id)).name;
				bool selected = (object2D_.selectedId_ == ref.id);

				if (ImGui::Selectable(name.c_str(), selected)) {

					object2D_.selectedId_ = ref.id;
					object3D_.selectedId_ = std::nullopt;
				}
			}
		}
	}
}

void ImGuiComponentManager::EditObject() {

	// 各objectの操作
	EditObject3D();
	EditObject2D();
}

void ImGuiComponentManager::SetImGuiFunc(EntityID entityId, std::function<void()> func) {

	object3D_.imguiFunc_[entityId] = func;
}

void ImGuiComponentManager::EditObject3D() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(transform3DManager_->GetComponent(object3D_.selectedId_.value()), "does not exist object3D:transform");
	ASSERT(materialManager_->GetComponent(object3D_.selectedId_.value()), "does not exist object3D:material");

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

void ImGuiComponentManager::Object3DInformation() {

	ImGui::Text("name: %s", entity3DManager_->GetNames().at(entity3DManager_->GetIndex(*object3D_.selectedId_)).name.c_str());
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

	model->ImGui();
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

void ImGuiComponentManager::EditObject2D() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(transform2DManager_->GetComponent(object2D_.selectedId_.value()), "does not exist object2D:transform");
	ASSERT(spriteMaterialManager_->GetComponent(object2D_.selectedId_.value()), "does not exist object2D:material");

	Object2DInformation();

	ImGui::Separator();

	if (ImGui::BeginTabBar("Object2DTabs")) {

		if (ImGui::BeginTabItem("Rendering")) {

			Object2DRenderingData();
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

		if (ImGui::BeginTabItem("Individual")) {

			if (Algorithm::Find(object2D_.imguiFunc_, *object2D_.selectedId_)) {

				EntityID id = *object2D_.selectedId_;
				object2D_.imguiFunc_.at(id)();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImGuiComponentManager::Object2DInformation() {

	ImGui::Text("name: %s", entity2DManager_->GetNames().at(entity2DManager_->GetIndex(*object2D_.selectedId_)).name.c_str());
	ImGui::Text("entityId: %d", *object2D_.selectedId_);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveObject2D(*object2D_.selectedId_);
		object2D_.selectedId_ = std::nullopt;
	}
}

void ImGuiComponentManager::Object2DRenderingData() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	SpriteComponent* sprite = spriteComponentManager_->GetComponent(*object2D_.selectedId_);

	sprite->ImGui(itemWidth_);
}

void ImGuiComponentManager::Object2DTransform() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	Transform2DComponent* transform = transform2DManager_->GetComponent(*object2D_.selectedId_);

	transform->ImGui(itemWidth_);
}

void ImGuiComponentManager::Object2DMaterial() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	SpriteMaterial* material = spriteMaterialManager_->GetComponent(*object2D_.selectedId_);

	material->ImGui();
}