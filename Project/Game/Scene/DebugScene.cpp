#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Component/Manager/ComponentManager.h>
#include <Game/Camera/Manager/CameraManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	DebugScene classMethods
//============================================================================

void DebugScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager
) {

	//========================================================================
	//	load
	//========================================================================

	asset->LoadModel("teapot");

	asset->LoadModel("walk");
	asset->LoadAnimation("walk", "walk");

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	float kMaxCount = 16;
	float spancing = 1.5f;
	for (uint32_t i = 0; i < kMaxCount; ++i) {
		for (uint32_t j = 0; j < kMaxCount; ++j) {

			EntityID id = ComponentManager::GetInstance()->CreateObject3D("walk", std::nullopt, "walk", "walkInstancing");
			auto transform = ComponentManager::GetInstance()->GetComponent<Transform3DComponent>(id);

			transform->translation = Vector3(i * spancing, 0.0f, j * spancing);
		}
	}
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();

	LineRenderer::GetInstance()->DrawGrid(16, 16.0f, Color::White());
}