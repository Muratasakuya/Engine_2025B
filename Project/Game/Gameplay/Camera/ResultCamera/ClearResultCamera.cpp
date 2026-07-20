#include "ClearResultCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/Line/LineRenderer.h>
#include <Engine/Utility/Json/JsonAdapter.h>
#include <Engine/Utility/Enum/EnumAdapter.h>
#include <Engine/Utility/Timer/GameTimer.h>

//============================================================================
//	ClearResultCamera classMethods
//============================================================================

void ClearResultCamera::Init() {

	initRotateX_ = transform_.GetEulerRotation().x;

	// json適用
	ApplyJson();

	// 初期化値設定
	currentState_ = State::Begin;
}

void ClearResultCamera::Update() {

	switch (currentState_) {
	case ClearResultCamera::State::Begin:

		UpdateAnimation();
		break;
	case ClearResultCamera::State::Rotate:

		UpdateRotate();
		break;
	}

	// 行列更新
	BaseCamera::UpdateView();
}

void ClearResultCamera::UpdateAnimation() {

	// 時間を進める
	animationTimer_.Update();

	// 座標を補間
	transform_.SetTranslation(SakuEngine::Vector3::Lerp(startPos_, targetPos_, animationTimer_.easedT_));

	// 補間が終了したら次に進める
	if (animationTimer_.IsReached()) {

		animationTimer_.Reset();
		currentState_ = State::Rotate;
		SakuEngine::Vector3 eulerRotate = transform_.GetEulerRotation();
		eulerRotate.x = eulerRotateX_;
		transform_.SetEulerRotation(eulerRotate);
	}
}

void ClearResultCamera::UpdateRotate() {

	// Y軸回転を加算
	SakuEngine::Vector3 eulerRotate = transform_.GetEulerRotation();
	eulerRotate.y += rotateSpeed_ * SakuEngine::GameTimer::GetDeltaTime();
	transform_.SetEulerRotation(eulerRotate);

	// オフセット距離
	SakuEngine::Vector3 offset = SakuEngine::Vector3::Transform(SakuEngine::Vector3(0.0f, 0.0f, -viewOffset_),
		SakuEngine::Matrix4x4::MakeRotateMatrix(transform_.GetEulerRotation()));
	// 座標を設定
	transform_.SetTranslation(viewPoint_ + offset);
}

void ClearResultCamera::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	SakuEngine::EnumAdapter<State>::Combo("state", &currentState_);

	BaseCamera::ImGui();

	ImGui::SeparatorText("Begin");

	if (ImGui::Button("Reste")) {

		animationTimer_.Reset();
	}

	ImGui::DragFloat3("startPos", &startPos_.x, 0.1f);
	ImGui::DragFloat3("targetPos", &targetPos_.x, 0.1f);

	animationTimer_.ImGui("Animation");

	ImGui::SeparatorText("Rotate");

	ImGui::DragFloat("rotateSpeed", &rotateSpeed_, 0.01f);
	if (ImGui::DragFloat("eulerRotateX", &eulerRotateX_, 0.01f)) {

		SakuEngine::Vector3 editEuler = transform_.GetEulerRotation();
		editEuler.x = eulerRotateX_;
		transform_.SetEulerRotation(editEuler);
	}
	ImGui::DragFloat3("viewPoint", &viewPoint_.x, 0.1f);
	ImGui::DragFloat3("viewOffset", &viewOffset_, 0.1f);

	switch (currentState_) {
	case ClearResultCamera::State::Begin: {

		SakuEngine::Vector3 editEuler = transform_.GetEulerRotation();
		editEuler.x = initRotateX_;
		editEuler.y = 0.0f;
		transform_.SetEulerRotation(editEuler);
		break;
	}
	case ClearResultCamera::State::Rotate: {

		SakuEngine::LineRenderer::GetInstance()->Get3D()->DrawSphere(8, 4.0f,
			viewPoint_, SakuEngine::Color::Cyan());
		SakuEngine::LineRenderer::GetInstance()->Get3D()->DrawLine(viewPoint_,
			transform_.GetTranslation(), SakuEngine::Color::Cyan());
		break;
	}
	}
}

void ClearResultCamera::ApplyJson() {

	Json data;
	if (!SakuEngine::JsonAdapter::LoadCheck("Camera/Clear/resultCameraParam.json", data)) {
		return;
	}

	animationTimer_.FromJson(data["AnimationTimer"]);
	startPos_ = SakuEngine::Vector3::FromJson(data["startPos_"]);
	targetPos_ = SakuEngine::Vector3::FromJson(data["targetPos_"]);

	rotateSpeed_ = data.value("rotateSpeed_", 0.1f);
	eulerRotateX_ = data.value("eulerRotateX", 1.0f);
	fovY_ = data.value("fovY_", 0.1f);
	farClip_ = data.value("farClip_", 0.1f);
	viewPoint_ = SakuEngine::Vector3::FromJson(data["viewPoint_"]);
	viewOffset_ = data.value("viewOffset_", 32.0f);
}

void ClearResultCamera::SaveJson() {

	Json data;

	animationTimer_.ToJson(data["AnimationTimer"]);
	data["startPos_"] = startPos_.ToJson();
	data["targetPos_"] = targetPos_.ToJson();

	data["rotateSpeed_"] = rotateSpeed_;
	data["eulerRotateX"] = transform_.GetEulerRotation().x;
	data["fovY_"] = fovY_;
	data["farClip_"] = farClip_;
	data["viewPoint_"] = viewPoint_.ToJson();
	data["viewOffset_"] = viewOffset_;

	SakuEngine::JsonAdapter::Save("Camera/Clear/resultCameraParam.json", data);
}
