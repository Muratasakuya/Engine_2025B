#include "SceneManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/ImGuiInspector.h>

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset, CameraManager* cameraManager,
	LightManager* lightManager, PostProcessSystem* postProcessSystem) {

	asset_ = nullptr;
	asset_ = asset;

	cameraManager_ = nullptr;
	cameraManager_ = cameraManager;

	lightManager_ = nullptr;
	lightManager_ = lightManager;

	postProcessSystem_ = nullptr;
	postProcessSystem_ = postProcessSystem;

	factory_ = std::make_unique<SceneFactory>();

	sceneTransition_ = std::make_unique<SceneTransition>();
	sceneTransition_->Init();

	LoadScene(scene);
	currentScene_->Init(asset_, cameraManager, lightManager, postProcessSystem);
}

void SceneManager::Update() {

	currentScene_->Update(this);

	sceneTransition_->Update();
}

void SceneManager::SwitchScene() {

	if (sceneTransition_->IsBeginTransitionFinished()) {

		isSceneSwitching_ = true;
		sceneTransition_->SetResetBeginTransition();
	}

	if (isSceneSwitching_) {

		LoadScene(nextScene_);
	}
}

void SceneManager::InitNextScene() {

	currentScene_->Init(asset_, cameraManager_,
		lightManager_, postProcessSystem_);
	isSceneSwitching_ = false;
}

void SceneManager::SetNextScene(Scene scene, std::unique_ptr<ITransition> transition) {

	nextScene_ = scene;

	sceneTransition_->SetTransition(std::move(transition));
}

void SceneManager::LoadScene(Scene scene) {

	currentScene_.reset();
	// 次のScene
	currentScene_ = factory_->Create(scene);

	// imgui選択をリセット
	ImGuiInspector::GetInstance()->Reset();
}