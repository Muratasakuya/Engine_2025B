#include "SceneManager.h"

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset, CameraManager* cameraManager,
	PostProcessManager* postProcessManager) {

	asset_ = nullptr;
	asset_ = asset;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	postProcessManager_ = nullptr;
	postProcessManager_ = postProcessManager;

	factory_ = std::make_unique<SceneFactory>();

	LoadScene(scene);
	currentScene_->Init(asset_, cameraManager, postProcessManager);
}

void SceneManager::Update() {

	currentScene_->Update(this);
}

void SceneManager::SwitchScene() {

	if (isSceneSwitching_) {
		LoadScene(nextScene_);
	}
}

void SceneManager::InitNextScene() {

	currentScene_->Init(asset_, cameraManager_, postProcessManager_);
	isSceneSwitching_ = false;
}

void SceneManager::SetNextScene(Scene scene) {

	nextScene_ = scene;
}

void SceneManager::LoadScene(Scene scene) {

	currentScene_.reset();
	// 次のScene
	currentScene_ = factory_->Create(scene);
}