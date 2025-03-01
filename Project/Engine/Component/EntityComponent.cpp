#include "EntityComponent.h"

//============================================================================
//	EntityComponent classMethods
//============================================================================
#include <Engine/Core/Debug/Assert.h>

// imgui
#include <imgui.h>

//============================================================================
//	EntityComponent classMethods
//============================================================================

EntityComponent* EntityComponent::instance_ = nullptr;

EntityComponent* EntityComponent::GetInstance() {

	if (instance_ == nullptr) {
		instance_ = new EntityComponent();
	}
	return instance_;
}

void EntityComponent::Finalize() {

	if (instance_ != nullptr) {

		delete instance_;
		instance_ = nullptr;
	}
}

EntityData* EntityComponent::AddComponent(const std::string& modelName,
	const std::optional<std::string>& animationName, const std::string& name) {

	// entity追加
	uint32_t entityID = nextID_++;
	entities_[entityID] = EntityData();
	EntityData* entityData = &entities_[entityID];
	entityData->id = entityID;

	// Animationを行うかどうか
	if (animationName.has_value()) {

		buffers_[entityID] = EntityAnimationBufferData();

		// buffer作成
		if (auto* animationBuffer = std::get_if<EntityAnimationBufferData>(&buffers_[entityID])) {

			animationBuffer->model = std::make_unique<AnimationModel>();
			animationBuffer->model->Init(modelName, *animationName, device_, asset_, srvManager_);

			animationBuffer->transform.Init(*animationName, asset_);
			animationBuffer->materials.resize(animationBuffer->model->GetMeshNum());
			entityData->materials.resize(animationBuffer->model->GetMeshNum());
			entityData->uvTransforms.resize(animationBuffer->model->GetMeshNum());
			for (auto& material : animationBuffer->materials) {

				material.CreateConstBuffer(device_);
			}
		}
	} else {

		buffers_[entityID] = EntityBufferData();

		// buffer作成
		if (auto* buffer = std::get_if<EntityBufferData>(&buffers_[entityID])) {

			buffer->model = std::make_unique<BaseModel>();
			buffer->model->Init(modelName, device_, asset_);

			buffer->transform.CreateConstBuffer(device_);
			buffer->materials.resize(buffer->model->GetMeshNum());
			entityData->materials.resize(buffer->model->GetMeshNum());
			entityData->uvTransforms.resize(buffer->model->GetMeshNum());
			for (auto& material : buffer->materials) {

				material.CreateConstBuffer(device_);
			}
		}
	}

	// 識別名
	std::string identifier = CheckName(name);
	entityIdentifiers_[entityID] = identifier;

	needsSorting_ = true;
	return entityData;
}

void EntityComponent::RemoveComponent(uint32_t entityID) {

	entities_.erase(entityID);
	buffers_.erase(entityID);
	entityIdentifiers_.erase(entityID);
	needsSorting_ = true;
}

void EntityComponent::Init(ID3D12Device* device, Asset* asset, SRVManager* srvManager) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;

	srvManager_ = nullptr;
	srvManager_ = srvManager;
}

void EntityComponent::Update() {

	// GPU転送
	for (auto& [entityID, entity] : entities_) {

		auto it = buffers_.find(entityID);
		if (auto* model = std::get_if<EntityBufferData>(&it->second)) {

			entities_[entityID].transform.UpdateMatrix();
			model->transform.TransferData(entities_[entityID].transform.matrix);
			for (uint32_t meshIndex = 0; meshIndex < model->materials.size(); ++meshIndex) {

				// uvの更新
				entities_[entityID].materials[meshIndex].uvTransform = Matrix4x4::MakeAffineMatrix(
					entities_[entityID].uvTransforms[meshIndex].scale,
					entities_[entityID].uvTransforms[meshIndex].rotate,
					entities_[entityID].uvTransforms[meshIndex].translate);
				model->materials[meshIndex].TransferData(entities_[entityID].materials[meshIndex]);
			}
			model->renderingData.drawEnable = entities_[entityID].renderingData.drawEnable;
			model->renderingData.blendMode = entities_[entityID].renderingData.blendMode;
		} else if (auto* animationModel = std::get_if<EntityAnimationBufferData>(&it->second)) {

			entities_[entityID].transform.UpdateMatrix();
			animationModel->transform.Update(entities_[entityID].transform.matrix);
			for (uint32_t meshIndex = 0; meshIndex < animationModel->materials.size(); ++meshIndex) {

				// uvの更新
				entities_[entityID].materials[meshIndex].uvTransform = Matrix4x4::MakeAffineMatrix(
					entities_[entityID].uvTransforms[meshIndex].scale,
					entities_[entityID].uvTransforms[meshIndex].rotate,
					entities_[entityID].uvTransforms[meshIndex].translate);
				animationModel->materials[meshIndex].TransferData(entities_[entityID].materials[meshIndex]);
			}
			animationModel->renderingData.drawEnable = entities_[entityID].renderingData.drawEnable;
			animationModel->renderingData.blendMode = entities_[entityID].renderingData.blendMode;
		}
	}
}

const std::unordered_map<BlendMode, std::vector<EntityComponent::EntityBufferPtr>>& EntityComponent::GetBuffers() const {

	if (needsSorting_) {
		SortEntitiesByBlendMode();
	}
	return sortedEntities_;
}

void EntityComponent::SortEntitiesByBlendMode() const {

	// 既存のデータをクリア
	sortedEntities_.clear();

	for (auto& [entityID, entityVariant] : buffers_) {
		if (auto* buffer = std::get_if<EntityBufferData>(&entityVariant)) {

			if (buffer && !buffer->renderingData.drawEnable) continue;

			sortedEntities_[buffer->renderingData.blendMode].push_back(
				std::variant<EntityBufferData*, EntityAnimationBufferData*>(const_cast<EntityBufferData*>(buffer)));
		} else if (auto* animatedBuffer = std::get_if<EntityAnimationBufferData>(&entityVariant)) {

			if (animatedBuffer && !animatedBuffer->renderingData.drawEnable) continue;

			sortedEntities_[animatedBuffer->renderingData.blendMode].push_back(
				std::variant<EntityBufferData*, EntityAnimationBufferData*>(const_cast<EntityAnimationBufferData*>(animatedBuffer)));
		}
	}
	// ソート処理完了
	needsSorting_ = false;
}

std::string EntityComponent::CheckName(const std::string& name) {

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

std::string EntityComponent::SplitBaseNameAndNumber(const std::string& name, int& number) {

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

void EntityComponent::ImGuiSelectObject() {

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

	if (entityIdentifiers_.size() != lastEntityCount_) {

		groupedEntities_.clear();
		for (const auto& [id, name] : entityIdentifiers_) {
			auto [baseName, number] = GetBaseNameAndNumber(name);
			groupedEntities_[baseName].emplace_back(number, id);
		}

		// 数字順にソートを行う処理
		for (auto& [_, objects] : groupedEntities_) {
			std::sort(objects.begin(), objects.end(), [](const auto& a, const auto& b) {
				return a.first < b.first;
				});
		}

		lastEntityCount_ = entityIdentifiers_.size();
	}

	// objectNameの表示
	for (auto& [baseName, objects] : groupedEntities_) {
		if (objects.size() == 1) {
			if (ImGui::Selectable(baseName.c_str(), selectedEntityId_ == objects[0].second)) {
				selectedEntityId_ = objects[0].second;
			}
		} else {
			if (ImGui::TreeNode(baseName.c_str())) {
				for (const auto& [number, id] : objects) {

					std::string label = baseName + std::to_string(number);
					if (ImGui::Selectable(label.c_str(), selectedEntityId_ == id)) {

						// idの更新処理
						selectedEntityId_ = id;
						selectedMaterialIndex_ = 0;
					}
				}
				ImGui::TreePop();
			}
		}
	}
}

void EntityComponent::ImGuiEdit() {

	if (!selectedEntityId_.has_value()) {
		return;
	}

	if (entities_.find(*selectedEntityId_) != entities_.end() &&
		entityIdentifiers_.find(*selectedEntityId_) != entityIdentifiers_.end()) {

		ImGui::SeparatorText("SelectedObject");
	} else {

		ASSERT(FALSE, "does not exist entity");
	}

	// entityの情報表示
	ImGuiEntityInformation();
	// 描画
	ImGuiEntityDraw();
	// Transform
	ImGuiEntityTransform();
	// Material
	ImGuiEntityMaterial();
}

void EntityComponent::ImGuiEntityInformation() {

	ImGui::Begin("Information");

	ImGui::Text("name: %s", entityIdentifiers_[*selectedEntityId_].c_str());
	ImGui::Text("entityId: %d", entities_[*selectedEntityId_].id);

	// 選択中のObjectの削除
	if (ImGui::Button("remove")) {

		RemoveComponent(*selectedEntityId_);
		selectedEntityId_ = std::nullopt;
	}

	ImGui::End();
}

void EntityComponent::ImGuiEntityDraw() {

	ImGui::Begin("Draw");

	if (ImGui::Checkbox("drawEnable", &entities_[*selectedEntityId_].renderingData.drawEnable)) {
		needsSorting_ = true;
	}

	const char* blendModeItems[] = {
				"Normal",     // kBlendModeNormal
				"Add",        // kBlendModeAdd
				"Subtract",   // kBlendModeSubtract
				"Multiply",   // kBlendModeMultiply
				"Screen"      // kBlendModeScreen
	};
	int blendIndex = static_cast<int>(entities_[*selectedEntityId_].renderingData.blendMode);

	ImGui::PushItemWidth(168.0f);
	if (ImGui::Combo("blendMode", &blendIndex, blendModeItems, IM_ARRAYSIZE(blendModeItems))) {
		entities_[*selectedEntityId_].renderingData.blendMode = static_cast<BlendMode>(blendIndex);
		needsSorting_ = true;
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

void EntityComponent::ImGuiEntityTransform() {

	ImGui::Begin("Transform");

	ImGui::PushItemWidth(168.0f);
	ImGui::DragFloat3("scale", &entities_[*selectedEntityId_].transform.scale.x, 0.01f);
	ImGui::DragFloat3("translate", &entities_[*selectedEntityId_].transform.translation.x, 0.01f);
	ImGui::PopItemWidth();

	ImGui::End();
}

void EntityComponent::ImGuiEntityMaterial() {

	ImGui::Begin("Material");

	ImGui::PushItemWidth(168.0f);
	if (ImGui::BeginCombo("Select Material", ("Material " + std::to_string(selectedMaterialIndex_)).c_str())) {
		for (size_t i = 0; i < entities_[*selectedEntityId_].materials.size(); ++i) {

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
	if (!entities_[*selectedEntityId_].materials.empty()) {

		selectedMaterialIndex_ = std::clamp(selectedMaterialIndex_, 0, static_cast<int>(entities_[*selectedEntityId_].materials.size() - 1));
		auto& mat = entities_[*selectedEntityId_].materials[selectedMaterialIndex_];

		/*============================================================================================================*/

		ImGui::Text("Editing Material %d", selectedMaterialIndex_);

		ImGui::SeparatorText("Color");

		ImGui::ColorEdit4("color", &mat.color.r);
		ImGui::Text("R:%4.3f G:%4.3f B:%4.3f A:%4.3f",
			mat.color.r, mat.color.g,
			mat.color.b, mat.color.a);

		ImGui::SeparatorText("Emission");

		ImGui::ColorEdit3("emissionColor", &mat.emissionColor.x);
		ImGui::Text("R:%4.3f G:%4.3f B:%4.3f",
			mat.emissionColor.x, mat.emissionColor.y,
			mat.emissionColor.z);
		ImGui::PushItemWidth(168.0f);
		ImGui::DragFloat("emissiveIntensity", &mat.emissiveIntensity, 0.01f);
		ImGui::PopItemWidth();

		/*============================================================================================================*/

		ImGui::SeparatorText("UV");

		ImGui::PushItemWidth(168.0f);
		ImGui::DragFloat2("scale##UV", &entities_[*selectedEntityId_].uvTransforms[selectedMaterialIndex_].scale.x, 0.01f);
		ImGui::SliderAngle("rotateZ##UV", &entities_[*selectedEntityId_].uvTransforms[selectedMaterialIndex_].rotate.z, 0.01f);
		ImGui::DragFloat2("translate##UV", &entities_[*selectedEntityId_].uvTransforms[selectedMaterialIndex_].translate.x, 0.01f);
		ImGui::PopItemWidth();

		/*============================================================================================================*/

		ImGui::SeparatorText("Lighting");

		ImGui::SliderInt("enableLighting", &mat.enableLighting, 0, 1);
		if (ImGui::SliderInt("phongReflection", &mat.enablePhongReflection, 0, 1)) {
			if (mat.enablePhongReflection) {

				mat.enableBlinnPhongReflection = 0;
			}
		}
		if (ImGui::SliderInt("blinnPhongReflection", &mat.enableBlinnPhongReflection, 0, 1)) {
			if (mat.enableBlinnPhongReflection) {

				mat.enablePhongReflection = 0;
			}
		}

		if (mat.enablePhongReflection ||
			mat.enableBlinnPhongReflection) {

			ImGui::ColorEdit3("specularColor", &mat.specularColor.x);
			ImGui::DragFloat("phongRefShininess", &mat.phongRefShininess, 0.01f);
		}

		/*============================================================================================================*/

		ImGui::SeparatorText("Shadow");

		ImGui::PushItemWidth(168.0f);
		ImGui::DragFloat("shadowRate", &mat.shadowRate, 0.01f, 0.0f, 8.0f);
		ImGui::PopItemWidth();
	}

	ImGui::End();
}