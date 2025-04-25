#include "BasePrimitiveEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>

//============================================================================
//	BasePrimitiveEffect classMethods
//============================================================================

void BasePrimitiveEffect::Init(const std::string& modelName,
	const std::string& textureName, const std::string& objectName,
	const std::string& groupName) {

	// 作成
	uint32_t id = GameObjectHelper::CreateEffect(
		modelName, textureName, objectName, groupName);

	// 各component取得
	transform_ = Component::GetComponent<EffectTransformComponent>(id);
	material_ = Component::GetComponent<EffectMaterialComponent>(id);
	mesh_ = Component::GetComponent<PrimitiveMeshComponent>(id);

	// defaultの設定
	// billboardを行う
	transform_->useBillboardMatrix_ = true;
	// 加算合成
	mesh_->SetBlendMode(BlendMode::kBlendModeAdd);

	// animationの初期化
	InitAnimation();
	// json適応
	ApplyJson();
}

void BasePrimitiveEffect::EditComponent() {

	// 各componentのimgui表示を行う
	if (ImGui::CollapsingHeader("Transform")) {

		transform_->ImGui(itemWidth_);
	}
	if (ImGui::CollapsingHeader("Material")) {

		material_->ImGui(itemWidth_);
	}
}

void BasePrimitiveEffect::OutsideEffect() {

	// 範囲外に飛ばす
	const Vector3 outsideTranslate = Vector3(0.0f, -18.0f, 0.0f);
	transform_->translation = outsideTranslate;
}

void BasePrimitiveEffect::ApplyComponent(const Json& data) {


	transform_->scale = JsonAdapter::ToObject<Vector3>(data["Transform"]["scale"]);
	transform_->rotation = JsonAdapter::ToObject<Quaternion>(data["Transform"]["rotation"]);
	material_->FromJson(data["Material"]);
}

void BasePrimitiveEffect::SaveComponent(Json& data) {

	// scaleとrotationのみ
	data["Transform"]["scale"] = JsonAdapter::FromObject<Vector3>(transform_->scale);
	data["Transform"]["rotation"] = JsonAdapter::FromObject<Quaternion>(transform_->rotation);
	material_->ToJson(data["Material"]);
}