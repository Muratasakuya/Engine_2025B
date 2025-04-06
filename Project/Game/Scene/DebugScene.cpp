#include "DebugScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessManager.h>
#include <Engine/Core/Component/User/ComponentHelper.h>
#include <Game/Camera/Manager/CameraManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	DebugScene classMethods
//============================================================================

void DebugScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] PostProcessManager* postProcessManager
) {

	//========================================================================
	//	load
	//========================================================================

	asset->LoadTexture("uvChecker");

	asset->LoadModel("teapot");
	asset->LoadModel("multiMaterial");

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessManager_ = postProcessManager;

	const std::vector<PostProcess> postProcesses = {

		PostProcess::Bloom,
	};

	postProcessManager_->Create(postProcesses);
	postProcessManager_->AddProcess(PostProcess::Bloom);

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	// sceneCameraにセット
	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

	const uint32_t kNumObject = 512;
	const float offset = 6.0f;
	const float offsetY = 4.0f;
	const uint32_t gridSize = static_cast<uint32_t>(std::sqrt(kNumObject));
	for (uint32_t index = 0; index < kNumObject; ++index) {

		EntityID id = GameObjectHelper::CreateObject3D("multiMaterial", "multiMaterial", "MultiMaterial");
		auto transform = Component::GetComponent<Transform3DComponent>(id);

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.z = z * offset;
	}

	for (uint32_t index = 0; index < kNumObject; ++index) {

		EntityID id = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot");
		auto transform = Component::GetComponent<Transform3DComponent>(id);

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.y = offsetY;
		transform->translation.z = z * offset;
	}
}

void DebugScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	update
	//========================================================================

	gameCamera_->Update();
}