#include "PlayerWeapon.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/Line/LineRenderer.h>
#include <Engine/Utility/Json/JsonAdapter.h>

//============================================================================
//	PlayerWeapon classMethods
//============================================================================

void PlayerWeapon::SetInitTransform() {

	TransformData().SetScale(initTransform_.GetScale());
	TransformData().SetEulerRotation(initTransform_.GetEulerRotation());
	TransformData().SetRotation(initTransform_.GetRotation());
	TransformData().SetTranslation(initTransform_.GetTranslation());
}

void PlayerWeapon::Update() {

	// 剣先の座標を更新する
	tipTranslation_ = SakuEngine::Vector3::Transform(tipOffset_, TransformData().GetMatrix().world);

	// 衝突情報更新
	Collider::UpdateAllBodies(TransformData());
}

void PlayerWeapon::DerivedImGui() {

	initTransform_.ImGui(itemWidth_);
	ImGui::Text("tipTranslation: %.3f,%.3f,%.3f", tipTranslation_.x,
		tipTranslation_.y, tipTranslation_.z);
	ImGui::DragFloat3("tipOffset", &tipOffset_.x, 0.01f);
	SetInitTransform();

	SakuEngine::LineRenderer::GetInstance()->Get3D()->DrawSphere(6, 0.8f, tipTranslation_, SakuEngine::Color::Cyan());

	Collider::ImGui(itemWidth_);
}

void PlayerWeapon::ApplyJson(const Json& data) {

	initTransform_.FromJson(data["InitTransform"]);
	tipOffset_ = SakuEngine::Vector3::FromJson(data.value("tipOffset_", Json()));
	SetInitTransform();

	GameObject3D::ApplyMaterial(data);
}

void PlayerWeapon::SaveJson(Json& data) {

	GameObject3D::SaveMaterial(data);
	Collider::SaveBodyOffset(data);
	initTransform_.ToJson(data["InitTransform"]);
	data["tipOffset_"] = tipOffset_.ToJson();
}
