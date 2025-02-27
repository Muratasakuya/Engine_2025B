#include "SceneManager.h"

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset, CameraManager* cameraManager) {

	asset_ = nullptr;
	asset_ = asset;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	factory_ = std::make_unique<SceneFactory>();

	LoadScene(scene);
	currentScene_->Init(asset_, cameraManager_);
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

	currentScene_->Init(asset_, cameraManager_);
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