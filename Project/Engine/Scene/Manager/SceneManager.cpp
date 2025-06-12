#include "SceneManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/ImGuiInspector.h>

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset,
	PostProcessSystem* postProcessSystem, SceneView* sceneView) {

	asset_ = nullptr;
	asset_ = asset;

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	postProcessSystem_ = nullptr;
	postProcessSystem_ = postProcessSystem;

	factory_ = std::make_unique<SceneFactory>();

	sceneTransition_ = std::make_unique<SceneTransition>();
	sceneTransition_->Init();

	LoadScene(scene);
	currentScene_->Init();
}

void SceneManager::Update() {

	currentScene_->Update();
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

	currentScene_->Init();
	isSceneSwitching_ = false;
}

void SceneManager::SetNextScene(Scene scene, std::unique_ptr<ITransition> transition) {

	nextScene_ = scene;
	sceneTransition_->SetTransition(std::move(transition));
}

void SceneManager::LoadScene(Scene scene) {

	currentScene_.reset();
	// 次のSceneを作成
	currentScene_ = factory_->Create(scene);
	currentScene_->SetPtr(asset_, postProcessSystem_, sceneView_, this);

	// imgui選択をリセット
	ImGuiInspector::GetInstance()->Reset();
}