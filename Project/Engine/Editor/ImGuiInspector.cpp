#include "ImGuiInspector.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Component/ECS/ComponentManager.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/MathUtils/Algorithm.h>

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

void ImGuiInspector::SetEntityManager(EntityRegistry* entity3DRegistry,
	EntityRegistry* effectRegistry,
	EntityRegistry* entity2DRegistry) {

	entity3DRegistry_ = nullptr;
	entity3DRegistry_ = entity3DRegistry;

	effectRegistry_ = nullptr;
	effectRegistry_ = effectRegistry;

	entity2DRegistry_ = nullptr;
	entity2DRegistry_ = entity2DRegistry;
}

void ImGuiInspector::SelectObject() {

	// groupの作成
	CreateGroup();
	// objectの選択
	SelectGroupedObject();
	SelectUnGroupedObject();
}

void ImGuiInspector::CreateGroup() {

	// entity取得
	const auto& current3DIds = entity3DRegistry_->GetIndexToEntity();
	const auto& currentEffectIds = effectRegistry_->GetIndexToEntity();
	const auto& current2DIds = entity2DRegistry_->GetIndexToEntity();

	// 変更があった場合に更新
	if (current3DIds != prevEntity3DIds_ ||
		currentEffectIds != prevEffectIds_ ||
		current2DIds != prevEntity2DIds_) {

		groupedEntities_.clear();

		// entityGroupの作成
		// 3D
		for (const uint32_t id : entity3DRegistry_->GetIndexToEntity()) {

			const auto& entity = entity3DRegistry_->GetNames()[entity3DRegistry_->GetIndex(id)];
			std::string group = entity.groupName.value_or("");
			groupedEntities_[group].push_back(EntityReference(EntityType::Object3D, id));
		}

		// effect
		for (const uint32_t id : effectRegistry_->GetIndexToEntity()) {

			const auto& entity = effectRegistry_->GetNames()[effectRegistry_->GetIndex(id)];
			std::string group = entity.groupName.value_or("");
			groupedEntities_[group].push_back(EntityReference(EntityType::Effect, id));
		}

		// 2D
		for (const uint32_t id : entity2DRegistry_->GetIndexToEntity()) {

			const auto& entity = entity2DRegistry_->GetNames()[entity2DRegistry_->GetIndex(id)];
			std::string group = entity.groupName.value_or("");
			groupedEntities_[group].push_back(EntityReference(EntityType::Object2D, id));
		}

		// 現在の状態を保存
		prevEntity3DIds_ = current3DIds;
		prevEffectIds_ = currentEffectIds;
		prevEntity2DIds_ = current2DIds;
	}
}

void ImGuiInspector::SelectGroupedObject() {

	// group:TreeNode:Selectable
	for (const auto& [groupName, entities] : groupedEntities_) {
		if (!groupName.empty()) {
			if (ImGui::TreeNode(groupName.c_str())) {

				// object3D表示
				bool has3D = std::any_of(entities.begin(), entities.end(), [](const auto& ref) {
					return ref.type == EntityType::Object3D;
					});
				if (has3D) {
					for (const auto& ref : entities) {
						if (ref.type != EntityType::Object3D) continue;

						const auto& name = entity3DRegistry_->GetNames().at(entity3DRegistry_->GetIndex(ref.id)).name;
						bool selected = (object3D_.selectedId_ == ref.id);

						if (ImGui::Selectable(name.c_str(), selected)) {

							object3D_.selectedId_ = ref.id;
							effect_.selectedId_ = std::nullopt;
							object2D_.selectedId_ = std::nullopt;
						}
					}
				}

				// effect表示
				bool hasEffect = std::any_of(entities.begin(), entities.end(), [](const auto& ref) {
					return ref.type == EntityType::Effect;
					});
				if (hasEffect) {
					for (const auto& ref : entities) {
						if (ref.type != EntityType::Effect) continue;

						const auto& name = effectRegistry_->GetNames().at(effectRegistry_->GetIndex(ref.id)).name;
						bool selected = (effect_.selectedId_ == ref.id);

						if (ImGui::Selectable(name.c_str(), selected)) {

							effect_.selectedId_ = ref.id;
							object3D_.selectedId_ = std::nullopt;
							object2D_.selectedId_ = std::nullopt;
						}
					}
				}

				// object2D表示
				bool has2D = std::any_of(entities.begin(), entities.end(), [](const auto& ref) {
					return ref.type == EntityType::Object2D;
					});
				if (has2D) {
					for (const auto& ref : entities) {
						if (ref.type != EntityType::Object2D) continue;

						const auto& name = entity2DRegistry_->GetNames().at(entity2DRegistry_->GetIndex(ref.id)).name;
						bool selected = (object2D_.selectedId_ == ref.id);

						if (ImGui::Selectable(name.c_str(), selected)) {

							object2D_.selectedId_ = ref.id;
							object3D_.selectedId_ = std::nullopt;
							effect_.selectedId_ = std::nullopt;
						}
					}
				}

				ImGui::TreePop();
			}
		}
	}
}

void ImGuiInspector::SelectUnGroupedObject() {

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

				const auto& name = entity3DRegistry_->GetNames().at(entity3DRegistry_->GetIndex(ref.id)).name;
				bool selected = (object3D_.selectedId_ == ref.id);

				if (ImGui::Selectable(name.c_str(), selected)) {

					object3D_.selectedId_ = ref.id;
					effect_.selectedId_ = std::nullopt;
					object2D_.selectedId_ = std::nullopt;
				}
			}
		}

		// effect表示
		bool hasEffect = std::any_of(ungroupedEntities.begin(), ungroupedEntities.end(), [](const auto& ref) {
			return ref.type == EntityType::Effect;
			});
		if (hasEffect) {
			for (const auto& ref : ungroupedEntities) {
				if (ref.type != EntityType::Effect) continue;

				const auto& name = effectRegistry_->GetNames().at(effectRegistry_->GetIndex(ref.id)).name;
				bool selected = (effect_.selectedId_ == ref.id);

				if (ImGui::Selectable(name.c_str(), selected)) {

					effect_.selectedId_ = ref.id;
					object3D_.selectedId_ = std::nullopt;
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

				const auto& name = entity2DRegistry_->GetNames().at(entity2DRegistry_->GetIndex(ref.id)).name;
				bool selected = (object2D_.selectedId_ == ref.id);

				if (ImGui::Selectable(name.c_str(), selected)) {

					object2D_.selectedId_ = ref.id;
					object3D_.selectedId_ = std::nullopt;
					effect_.selectedId_ = std::nullopt;
				}
			}
		}
	}
}

void ImGuiInspector::EditObject() {

	// 各objectの操作
	EditObject3D();
	EditEffect();
	EditObject2D();
}

void ImGuiInspector::Reset() {

	object2D_.selectedId_ = std::nullopt;
	object3D_.selectedId_ = std::nullopt;
}

void ImGuiInspector::SetImGuiFunc(uint32_t entityId, std::function<void()> func) {

	object3D_.imguiFunc_[entityId] = func;
}

void ImGuiInspector::EditObject3D() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(Component::GetComponent<Transform3DComponent>(object3D_.selectedId_.value()), "does not exist object3D:transform");
	ASSERT(Component::GetComponent<MaterialComponent>(object3D_.selectedId_.value()), "does not exist object3D:material");

	Object3DInformation();

	ImGui::Separator();

	if (ImGui::BeginTabBar("Object3DTabs")) {

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

				uint32_t id = *object3D_.selectedId_;
				object3D_.imguiFunc_.at(id)();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImGuiInspector::Object3DInformation() {

	ImGui::Text("name: %s", entity3DRegistry_->GetNames().at(entity3DRegistry_->GetIndex(*object3D_.selectedId_)).name.c_str());
	ImGui::Text("entityId: %d", *object3D_.selectedId_);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveObject3D(*object3D_.selectedId_);
		object3D_.selectedId_ = std::nullopt;
	}
}

void ImGuiInspector::Object3DTransform() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	Transform3DComponent* transform = Component::GetComponent<Transform3DComponent>(*object3D_.selectedId_);

	transform->ImGui(itemWidth_);
}

void ImGuiInspector::Object3DMaterial() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	std::vector<MaterialComponent*> materials = Component::GetComponentList<MaterialComponent>(*object3D_.selectedId_);

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

void ImGuiInspector::EditEffect() {

	if (!effect_.selectedId_.has_value()) {
		return;
	}

	ASSERT(Component::GetComponent<EffectTransformComponent>(effect_.selectedId_.value()), "does not exist effect:transform");
	ASSERT(Component::GetComponent<EffectMaterialComponent>(effect_.selectedId_.value()), "does not exist effect:material");

	EffectInformation();

	ImGui::Separator();

	if (ImGui::BeginTabBar("EffectTabs")) {
		if (ImGui::BeginTabItem("Mesh")) {

			EffectMesh();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Transform")) {

			EffectTransform();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			EffectMaterial();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Individual")) {

			if (Algorithm::Find(effect_.imguiFunc_, *effect_.selectedId_)) {

				uint32_t id = *effect_.selectedId_;
				effect_.imguiFunc_.at(id)();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImGuiInspector::EffectInformation() {

	ImGui::Text("name: %s", effectRegistry_->GetNames().at(effectRegistry_->GetIndex(*effect_.selectedId_)).name.c_str());
	ImGui::Text("entityId: %d", *effect_.selectedId_);

	// 選択中のEffectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveEffect(*effect_.selectedId_);
		effect_.selectedId_ = std::nullopt;
	}
}

void ImGuiInspector::EffectMesh() {

	if (!effect_.selectedId_.has_value()) {
		return;
	}

	PrimitiveMeshComponent* primitiveMesh = Component::GetComponent<PrimitiveMeshComponent>(*effect_.selectedId_);

	primitiveMesh->ImGui(itemWidth_);
}

void ImGuiInspector::EffectTransform() {

	if (!effect_.selectedId_.has_value()) {
		return;
	}

	EffectTransformComponent* transform = Component::GetComponent<EffectTransformComponent>(*effect_.selectedId_);

	transform->ImGui(itemWidth_);
}

void ImGuiInspector::EffectMaterial() {

	if (!effect_.selectedId_.has_value()) {
		return;
	}

	EffectMaterialComponent* material = Component::GetComponent<EffectMaterialComponent>(*effect_.selectedId_);

	material->ImGui(itemWidth_);
}

void ImGuiInspector::EditObject2D() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(Component::GetComponent<Transform2DComponent>(object2D_.selectedId_.value()), "does not exist object2D:transform");
	ASSERT(Component::GetComponent<SpriteMaterial>(object2D_.selectedId_.value()), "does not exist object2D:material");

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

				uint32_t id = *object2D_.selectedId_;
				object2D_.imguiFunc_.at(id)();
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void ImGuiInspector::Object2DInformation() {

	ImGui::Text("name: %s", entity2DRegistry_->GetNames().at(entity2DRegistry_->GetIndex(*object2D_.selectedId_)).name.c_str());
	ImGui::Text("entityId: %d", *object2D_.selectedId_);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveObject2D(*object2D_.selectedId_);
		object2D_.selectedId_ = std::nullopt;
	}
}

void ImGuiInspector::Object2DRenderingData() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	SpriteComponent* sprite = Component::GetComponent<SpriteComponent>(*object2D_.selectedId_);

	sprite->ImGui(itemWidth_);
}

void ImGuiInspector::Object2DTransform() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	Transform2DComponent* transform = Component::GetComponent<Transform2DComponent>(*object2D_.selectedId_);

	transform->ImGui(itemWidth_);
}

void ImGuiInspector::Object2DMaterial() {

	if (!object2D_.selectedId_.has_value()) {
		return;
	}

	SpriteMaterial* material = Component::GetComponent<SpriteMaterial>(*object2D_.selectedId_);

	material->ImGui();
}