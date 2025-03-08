#include "ComponentImGui.h"

//============================================================================
//	imgui
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Component/Manager/ComponentManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	ComponentImGui classMethods
//============================================================================

void ComponentImGui::Init(EntityManager* entityManager, Transform3DManager* transform3DManager,
	MaterialManager* materialManager, ModelComponentManager* modelComponentManager) {

	entityManager_ = nullptr;
	entityManager_ = entityManager;

	transform3DManager_ = nullptr;
	transform3DManager_ = transform3DManager;

	materialManager_ = nullptr;
	materialManager_ = materialManager;

	modelComponentManager_ = nullptr;
	modelComponentManager_ = modelComponentManager;
}

void ComponentImGui::AddComponent(EntityID id, const std::function<void()>& func) {

	object3DImGui_[id] = func;
}

void ComponentImGui::RemoveComponent(EntityID id) {

	object3DImGui_.erase(id);
}

void ComponentImGui::SelectObject3D() {

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

void ComponentImGui::EditObject3D() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ASSERT(transform3DManager_->GetComponent(*object3D_.selectedId_), "does not exist object3D:transform");
	ASSERT(!materialManager_->GetComponentList(*object3D_.selectedId_).empty(), "does not exist object3D:material");

	ImGui::SeparatorText("SelectedObject");

	// Object詳細、操作
	Object3DInformation();
	Object3DRenderingData();
	Object3DTransform();
	Object3DMaterial();

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	if (object3DImGui_[*object3D_.selectedId_]) {

		ImGui::Begin("Individual");

		object3DImGui_[*object3D_.selectedId_]();

		ImGui::End();
	}
}

void ComponentImGui::Object3DInformation() {

	ImGui::Begin("Information");

	ImGui::Text("name: %s", entityManager_->GetNames().at(*object3D_.selectedId_).c_str());
	ImGui::Text("entityId: %d", *object3D_.selectedId_);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		ComponentManager::GetInstance()->RemoveObject3D(*object3D_.selectedId_);
		object3D_.selectedId_ = std::nullopt;
	}

	ImGui::End();
}

void ComponentImGui::Object3DRenderingData() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ImGui::Begin("Draw");

	ModelComponent* model = modelComponentManager_->GetComponent(*object3D_.selectedId_);

	if (ImGui::Checkbox("drawEnable", &model->renderingData.drawEnable)) {
	}

	const char* blendModeItems[] = {
				"Normal",     // kBlendModeNormal
				"Add",        // kBlendModeAdd
				"Subtract",   // kBlendModeSubtract
				"Multiply",   // kBlendModeMultiply
				"Screen"      // kBlendModeScreen
	};
	int blendIndex = static_cast<int>(model->renderingData.blendMode);

	ImGui::PushItemWidth(168.0f);
	if (ImGui::Combo("blendMode", &blendIndex, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {

		model->renderingData.blendMode = static_cast<BlendMode>(blendIndex);
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

void ComponentImGui::Object3DTransform() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ImGui::Begin("Transform");

	Transform3DComponent* transform = transform3DManager_->GetComponent(*object3D_.selectedId_);

	ImGui::PushItemWidth(168.0f);
	ImGui::DragFloat3("scale", &transform->scale.x, 0.01f);
	ImGui::DragFloat3("translate", &transform->translation.x, 0.01f);
	ImGui::PopItemWidth();

	ImGui::End();
}

void ComponentImGui::Object3DMaterial() {

	if (!object3D_.selectedId_.has_value()) {
		return;
	}

	ImGui::Begin("Material");

	std::vector<Material*> materials = materialManager_->GetComponentList(*object3D_.selectedId_);

	ImGui::PushItemWidth(168.0f);
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
		auto& mat = materials[selectedMaterialIndex_];

		/*============================================================================================================*/

		ImGui::Text("Editing Material %d", selectedMaterialIndex_);

		ImGui::SeparatorText("Color");

		ImGui::ColorEdit4("color", &mat->color.r);
		ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
			mat->color.r, mat->color.g,
			mat->color.b, mat->color.a);

		ImGui::SeparatorText("Emission");

		ImGui::ColorEdit3("emissionColor", &mat->emissionColor.x);
		ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
			mat->emissionColor.x, mat->emissionColor.y,
			mat->emissionColor.z);
		ImGui::PushItemWidth(168.0f);
		ImGui::DragFloat("emissiveIntensity", &mat->emissiveIntensity, 0.01f);
		ImGui::PopItemWidth();

		/*============================================================================================================*/

		ImGui::SeparatorText("Lighting");

		ImGui::SliderInt("enableLighting", &mat->enableLighting, 0, 1);
		if (ImGui::SliderInt("phongReflection", &mat->enablePhongReflection, 0, 1)) {
			if (mat->enablePhongReflection) {

				mat->enableBlinnPhongReflection = 0;
			}
		}
		if (ImGui::SliderInt("blinnPhongReflection", &mat->enableBlinnPhongReflection, 0, 1)) {
			if (mat->enableBlinnPhongReflection) {

				mat->enablePhongReflection = 0;
			}
		}

		if (mat->enablePhongReflection ||
			mat->enableBlinnPhongReflection) {

			ImGui::ColorEdit3("specularColor", &mat->specularColor.x);
			ImGui::DragFloat("phongRefShininess", &mat->phongRefShininess, 0.01f);
		}

		/*============================================================================================================*/

		ImGui::SeparatorText("Shadow");

		ImGui::PushItemWidth(168.0f);
		ImGui::DragFloat("shadowRate", &mat->shadowRate, 0.01f, 0.0f, 8.0f);
		ImGui::PopItemWidth();
	}

	ImGui::End();
}