#include "SceneManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/ImGuiObjectEditor.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Lib/Adapter/EnumAdapter.h>

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset,
	PostProcessSystem* postProcessSystem, SceneView* sceneView) :IGameEditor("SceneManager") {

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

		LoadScene(nextSceneType_);
	}
}

void SceneManager::InitNextScene() {

	currentScene_->Init();
	isSceneSwitching_ = false;
}

void SceneManager::SetNextScene(Scene scene, std::unique_ptr<ITransition> transition) {

	nextSceneType_ = scene;
	sceneTransition_->SetTransition(std::move(transition));
}

void SceneManager::ImGui() {

	ImGui::Text("CurrentScene : %s", EnumAdapter<Scene>::ToString(currentSceneType_));

	// 選択
	static Scene selected = currentSceneType_;
	if (EnumAdapter<Scene>::Combo("Select Scene", &selected)) {
	}

	if (ImGui::Button("Apply") && selected != currentSceneType_ && !isSceneSwitching_) {

		isSceneSwitching_ = true;
		nextSceneType_ = selected;
	}
}

void SceneManager::LoadScene(Scene scene) {

	currentScene_.reset();
	// 次のSceneを作成
	currentSceneType_ = scene;
	currentScene_ = factory_->Create(scene);
	currentScene_->SetPtr(asset_, postProcessSystem_, sceneView_, this);

	// imgui選択をリセット
	ImGuiObjectEditor::GetInstance()->Reset();
	// すべてのオブジェクトを破棄
	ObjectManager::GetInstance()->DestroyAll();
}