#include "TitleViewCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/Line/LineRenderer.h>
#include <Engine/Utility/Json/JsonAdapter.h>
#include <Engine/Utility/Enum/EnumAdapter.h>
#include <Engine/Utility/Timer/GameTimer.h>

//============================================================================
//	TitleViewCamera classMethods
//============================================================================

void TitleViewCamera::Init() {

	initRotateX_ = transform_.GetEulerRotation().x;

	// json適用
	ApplyJson();
}

void TitleViewCamera::Update() {

	// Y軸回転を加算
	SakuEngine::Vector3 eulerRotate = transform_.GetEulerRotation();
	eulerRotate.y += rotateSpeed_ * SakuEngine::GameTimer::GetDeltaTime();
	transform_.SetEulerRotation(eulerRotate);

	// オフセット距離
	SakuEngine::Vector3 offset = SakuEngine::Vector3::Transform(SakuEngine::Vector3(0.0f, 0.0f, -viewOffset_),
		SakuEngine::Matrix4x4::MakeRotateMatrix(transform_.GetEulerRotation()));
	// 座標を設定
	transform_.SetTranslation(viewPoint_ + offset);

	// 行列更新
	BaseCamera::UpdateView();
}

void TitleViewCamera::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	BaseCamera::ImGui();

	ImGui::SeparatorText("Rotate");

	ImGui::DragFloat("rotateSpeed", &rotateSpeed_, 0.01f);
	if (ImGui::DragFloat("eulerRotateX", &eulerRotateX_, 0.01f)) {

		SakuEngine::Vector3 editEuler = transform_.GetEulerRotation();
		editEuler.x = eulerRotateX_;
		transform_.SetEulerRotation(editEuler);
	}
	ImGui::DragFloat3("viewPoint", &viewPoint_.x, 0.1f);
	ImGui::DragFloat3("viewOffset", &viewOffset_, 0.1f);

	SakuEngine::LineRenderer::GetInstance()->Get3D()->DrawSphere(8, 4.0f,
		viewPoint_, SakuEngine::Color::Cyan());
	SakuEngine::LineRenderer::GetInstance()->Get3D()->DrawLine(viewPoint_,
		transform_.GetTranslation(), SakuEngine::Color::Cyan());
}

void TitleViewCamera::ApplyJson() {

	Json data;
	if (!SakuEngine::JsonAdapter::LoadCheck("Camera/Title/titleViewCameraParam.json", data)) {
		return;
	}

	rotateSpeed_ = data.value("rotateSpeed_", 0.1f);
	eulerRotateX_ = data.value("eulerRotateX", 1.0f);
	SakuEngine::Vector3 eulerRotate = transform_.GetEulerRotation();
	eulerRotate.x = eulerRotateX_;
	transform_.SetEulerRotation(eulerRotate);
	fovY_ = data.value("fovY_", 0.1f);
	farClip_ = data.value("farClip_", 0.1f);
	viewPoint_ = SakuEngine::Vector3::FromJson(data["viewPoint_"]);
	viewOffset_ = data.value("viewOffset_", 32.0f);
}

void TitleViewCamera::SaveJson() {

	Json data;

	data["rotateSpeed_"] = rotateSpeed_;
	data["eulerRotateX"] = transform_.GetEulerRotation().x;
	data["fovY_"] = fovY_;
	data["farClip_"] = farClip_;
	data["viewPoint_"] = viewPoint_.ToJson();
	data["viewOffset_"] = viewOffset_;

	SakuEngine::JsonAdapter::Save("Camera/Title/titleViewCameraParam.json", data);
}
