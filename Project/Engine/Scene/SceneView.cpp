#include "SceneView.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>

//============================================================================
//	SceneView classMethods
//============================================================================

void SceneView::InitCamera() {

	// 2D
	camera2D_ = std::make_unique<Camera2D>();
	camera2D_->Init();

#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Init();
#endif
}

void SceneView::Init() {

	// scene情報初期化
	InitCamera();

	dither_.Init();
}

void SceneView::Update() {

	// scene更新
	UpdateCamera();
	UpdateLight();
}

void SceneView::UpdateCamera() {

	// 3Dカメラの更新は各sceneクラスで行う

#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	debugCamera_->Update();
#endif
}

void SceneView::UpdateLight() {

	// ライトの更新はエンジン側で行う
	punctualLight_.value()->Update();

	// pointLight、spotLightのデバッグ表示
#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	// point
	DisplayPointLight();
	// spot
	DisplaySpotLight();
#endif
}

void SceneView::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	if (ImGui::BeginTabBar("SceneTab")) {

		if (ImGui::BeginTabItem("Camera")) {

			EditCamera();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Light")) {

			EditLight();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Dither")) {

			dither_.ImGui();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}

void SceneView::EditCamera() {

	if (ImGui::CollapsingHeader("ActiveCamera")) {

		activeCamera3D_.value()->ImGui();

	}

	if (ImGui::CollapsingHeader("DebugCamera")) {

		debugCamera_->ImGui();
	}
}

void SceneView::EditLight() {

	punctualLight_.value()->ImGui();
}

void SceneView::SetCamera(BaseCamera* gameCamera) {

	// カメラのセット
	activeCamera3D_ = std::nullopt;
	activeCamera3D_ = gameCamera;
}

void SceneView::SetLight(PunctualLight* gameLight) {

	// ライトのセット
	punctualLight_ = std::nullopt;
	punctualLight_ = gameLight;
}

void SceneView::DisplayPointLight() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	const auto& light = punctualLight_.value();

	// 規定値
	const int sphereDivision = 8;
	const float sphereRadius = 0.12f;
	const Color sphereColor = Color(
		light->point.color.r,
		light->point.color.g,
		light->point.color.b,
		0.8f); // 薄く表示する

	// 球で描画
	lineRenderer->DrawSphere(
		sphereDivision,
		sphereRadius,
		light->point.pos,
		sphereColor);
}

void SceneView::DisplaySpotLight() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	const auto& light = punctualLight_.value();

	const float coneLength = 2.0f;
	const Color coneColor = Color(
		light->spot.color.r,
		light->spot.color.g,
		light->spot.color.b,
		0.8f);

	const Vector3 pos = light->spot.pos;
	const Vector3 dir = Vector3::Normalize(light->spot.direction);

	const int coneDivision = 4;
	const float radius = coneLength * std::tanf(light->spot.cosAngle * 0.5f);
	Vector3 baseCenter = pos + dir * coneLength;

	for (uint32_t index = 0; index < coneDivision; ++index) {

		float theta1 = static_cast<float>(index) / coneDivision * std::numbers::pi_v<float>*2.0f;
		float theta2 = static_cast<float>(index + 1) / coneDivision * std::numbers::pi_v<float>*2.0f;

		Vector3 p1 = baseCenter + Vector3(cosf(theta1), 0, sinf(theta1)) * radius;
		Vector3 p2 = baseCenter + Vector3(cosf(theta2), 0, sinf(theta2)) * radius;

		// coneの形状で描画
		lineRenderer->DrawLine3D(p1, p2, coneColor);
		lineRenderer->DrawLine3D(pos, p1, coneColor);
	}
}