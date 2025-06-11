#include "LightManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>

//============================================================================
//	LightManager classMethods
//============================================================================

void LightManager::ImGui() {

	if (gameLight_.has_value()) {

		// 現在使われているライトの表示
		gameLight_.value()->ImGui();
	}
}

void LightManager::Update() {

	// 正規化
	gameLight_.value()->directional.direction =
		Vector3::Normalize(gameLight_.value()->directional.direction);
	gameLight_.value()->spot.direction=
		Vector3::Normalize(gameLight_.value()->spot.direction);

	if (!gameLight_.has_value()) {
		return;
	}

	// pointLight、spotLightのデバッグ表示
#if defined(_DEBUG) || defined(_DEVELOPBUILD)
	// point
	//DisplayPointLight();
	// spot
	//DisplaySpotLight();
#endif
}

void LightManager::SetLight(PunctualLight* gameLight) {

	// ライトのセット
	gameLight_ = std::nullopt;
	gameLight_ = gameLight;
}

PunctualLight* LightManager::GetLight() const {

	if (!gameLight_.has_value()) {
		ASSERT(FALSE, "un setting gameLight");
		return nullptr;
	}
	return *gameLight_;
}

void LightManager::DisplayPointLight() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	// 規定値
	const int sphereDivision = 8;
	const float sphereRadius = 0.12f;
	const Color sphereColor = Color(
		gameLight_.value()->point.color.r,
		gameLight_.value()->point.color.g,
		gameLight_.value()->point.color.b,
		0.8f); // 薄く表示する

	// 球で描画
	lineRenderer->DrawSphere(
		sphereDivision,
		sphereRadius,
		gameLight_.value()->point.pos,
		sphereColor);
}

void LightManager::DisplaySpotLight() {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	const float coneLength = 2.0f;
	const Color coneColor = Color(
		gameLight_.value()->spot.color.r,
		gameLight_.value()->spot.color.g,
		gameLight_.value()->spot.color.b,
		0.8f);

	const Vector3 pos = gameLight_.value()->spot.pos;
	const Vector3 dir = Vector3::Normalize(gameLight_.value()->spot.direction);

	const int coneDivision = 4;
	const float radius = coneLength * std::tanf(gameLight_.value()->spot.cosAngle * 0.5f);
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