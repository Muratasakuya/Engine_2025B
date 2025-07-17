#include "GameObject3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Editor/ImGuiObjectEditor.h>

//============================================================================
//	GameObject3D classMethods
//============================================================================

void GameObject3D::Init(const std::string& modelName, const std::string& name,
	const std::string& groupName, const std::optional<std::string>& animationName) {

	// object作成
	objectId_ = objectManager_->CreateObjects(modelName, name, groupName, animationName);

	// data取得
	transform_ = objectManager_->GetData<Transform3D>(objectId_);
	materials_ = objectManager_->GetData<Material, true>(objectId_);
	tag_ = objectManager_->GetData<ObjectTag>(objectId_);

	// animationが存在する場合のみ処理する
	if (animationName.has_value()) {

		animation_ = objectManager_->GetData<SkinnedAnimation>(objectId_);
	}

	// editorに登録
	ImGuiObjectEditor::GetInstance()->Registerobject(objectId_, this);

	// 継承先のinit実装
	DerivedInit();
}

void GameObject3D::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	if (ImGui::BeginTabBar("GameObject3DTab")) {

		if (ImGui::BeginTabItem("Transform")) {

			TransformImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Material")) {

			MaterialImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animation")) {

			AnimationImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Collision")) {

			Collider::ImGui(itemWidth_);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Derived")) {

			// 継承先のimgui実装
			DerivedImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}

void GameObject3D::ApplyMaterial(const Json& data) {

	for (uint32_t meshIndex = 0; meshIndex < static_cast<uint32_t>((*materials_).size()); ++meshIndex) {

		(*materials_)[meshIndex].FromJson(data[Algorithm::GetIndexLabel("Material", meshIndex)]);
	}
}

void GameObject3D::SaveMaterial(Json& data) {

	for (uint32_t meshIndex = 0; meshIndex < static_cast<uint32_t>((*materials_).size()); ++meshIndex) {

		(*materials_)[meshIndex].ToJson(data[Algorithm::GetIndexLabel("Material", meshIndex)]);
	}
}

void GameObject3D::TransformImGui() {

	transform_->ImGui(itemWidth_);
}

void GameObject3D::MaterialImGui() {

	if (ImGui::BeginCombo("SelectMaterial",
		("Material " + std::to_string(selectedMaterialIndex_)).c_str())) {
		for (int i = 0; i < static_cast<int>((*materials_).size()); ++i) {

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

	selectedMaterialIndex_ = std::clamp(selectedMaterialIndex_, 0, static_cast<int>((*materials_).size() - 1));
	if (!(*materials_).empty()) {

		(*materials_)[selectedMaterialIndex_].ImGui(itemWidth_);
	}
}

void GameObject3D::AnimationImGui() {

	if (!animation_) {

		ImGui::Text("animation haven't...");
		return;
	}

	animation_->ImGui(itemWidth_);
}

void GameObject3D::SetNextAnimation(const std::string& nextAnimationName,
	bool loopAnimation, float transitionDuration) {

	// 次のanimationを設定
	animation_->SwitchAnimation(nextAnimationName, loopAnimation, transitionDuration);
}

void GameObject3D::ResetAnimation() {

	animation_->ResetAnimation();
}

Vector3 GameObject3D::GetJointWorldPos(const std::string& jointName) const {

	// animationが存在しない場合は空のVector3を返す
	if (!animation_) {
		return Vector3();
	}

	// 骨の情報を取得
	const Skeleton& skeleton = animation_->GetSkeleton();

	// 指定されたjointNameの骨が存在するか確認
	if (!Algorithm::Find(skeleton.jointMap, jointName)) {
		return Vector3();
	}
	// jointNameに対応するindexを取得
	uint32_t jointIndex = skeleton.jointMap.at(jointName);

	// model空間の行列を取得
	const Matrix4x4& jointToModel = skeleton.joints[jointIndex].skeletonSpaceMatrix;

	// 行列を合成する
	Matrix4x4 world = jointToModel * transform_->matrix.world;

	// 平行移動成分でワールド座標を返す
	return world.GetTranslationValue();
}

const Transform3D* GameObject3D::GetJointTransform(const std::string& jointName) const {

	return animation_ ? animation_->FindJointTransform(jointName) : nullptr;
}

void GameObject3D::SetColor(const Color& color, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].color = color;
	} else {

		for (auto& material : *materials_) {

			material.color = color;
		}
	}
}

void GameObject3D::SetAlpha(float alpha, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].color.a = alpha;
	} else {

		for (auto& material : *materials_) {

			material.color.a = alpha;
		}
	}
}

void GameObject3D::SetCastShadow(bool cast, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].castShadow = cast;
	} else {

		for (auto& material : *materials_) {

			material.castShadow = cast;
		}
	}
}

void GameObject3D::SetTextureName(const std::string& textureName, std::optional<uint32_t> meshIndex) {

	// meshIndexが設定されている場合のみ指定して設定
	if (meshIndex.has_value()) {

		(*materials_)[meshIndex.value()].SetTextureName(textureName);
	} else {

		for (auto& material : *materials_) {

			material.SetTextureName(textureName);
		}
	}
}