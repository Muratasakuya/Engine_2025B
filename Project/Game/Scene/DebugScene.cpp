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

	//========================================================================
	//	init
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	float kMaxCount = 24;
	float spancing = 4.0f;

	for (uint32_t i = 0; i < kMaxCount; ++i) {
		for (uint32_t j = 0; j < kMaxCount; ++j) {

			EntityID id = ComponentManager::GetInstance()->CreateObject3D("teapot", std::nullopt, "teapot", "teapotInstancing");
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
}