#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Component/User/ComponentHelper.h>
#include <Game/Camera/Manager/CameraManager.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] PostProcessManager* postProcessManager
) {

	//========================================================================
	//	load
	//========================================================================

	// Environment
	asset->LoadModel("stageField");
	asset->LoadModel("teapot");
	asset->LoadModel("multiMaterial");
	asset->LoadModel("suzanne");

	//========================================================================
	//	camera
	//========================================================================

	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Init();

	cameraManager->SetCamera(gameCamera_.get());

	//========================================================================
	//	initObject
	//========================================================================

	object3DEditor_ = std::make_unique<Object3DEditor>();
	object3DEditor_->Init(asset);

	stageField_ = std::make_unique<StageField>();
	stageField_->Init();

	const uint32_t kNumObject = 320;
	const float offset = 6.0f;
	const float offsetY = 4.0f;
	const uint32_t gridSize = static_cast<uint32_t>(std::sqrt(kNumObject));
	for (uint32_t index = 0; index < kNumObject; ++index) {

		uint32_t id = GameObjectHelper::CreateObject3D("multiMaterial", "multiMaterial", "MultiMaterial");
		auto transform = Component::GetComponent<Transform3DComponent>(id);

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.z = z * offset;
	}

	for (uint32_t index = 0; index < kNumObject; ++index) {

		uint32_t id = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot");
		auto transform = Component::GetComponent<Transform3DComponent>(id);

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.y = offsetY;
		transform->translation.z = z * offset;
	}

	for (uint32_t index = 0; index < kNumObject; ++index) {

		uint32_t id = GameObjectHelper::CreateObject3D("suzanne", "suzanne", "Suzanne");
		auto transform = Component::GetComponent<Transform3DComponent>(id);

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.y = offsetY * 2.0f;
		transform->translation.z = z * offset;
	}
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	gameCamera_->Update();

	stageField_->Update();
}