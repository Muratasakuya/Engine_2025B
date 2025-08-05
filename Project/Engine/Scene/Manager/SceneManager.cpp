#include "SceneManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Editor/ImGuiObjectEditor.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Utility/EnumAdapter.h>

//============================================================================
//	SceneManager classMethods
//============================================================================

SceneManager::SceneManager(Scene scene, Asset* asset,
	PostProcessSystem* postProcessSystem, SceneView* sceneView) :IGameEditor("SceneManager") {

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	asset_ = nullptr;
	asset_ = asset;

	postProcessSystem_ = nullptr;
	postProcessSystem_ = postProcessSystem;

	factory_ = std::make_unique<SceneFactory>();

	sceneTransition_ = std::make_unique<SceneTransition>();
	sceneTransition_->Init();

	// 最初のシーンファイルを読みこみ
	asset->LoadSceneAsync(scene, AssetLoadType::Synch);
	// 最初のシーン以外を非同期で読み込む
	for (uint32_t index = 0; index < EnumAdapter<Scene>::GetEnumCount(); ++index) {

		// 同じシーンは処理しない
		if (index == static_cast<uint32_t>(scene)) {
			continue;
		}
		asset->LoadSceneAsync(EnumAdapter<Scene>::GetValue(index), AssetLoadType::Async);
	}

	// 最初のシーンを読み込んで初期化
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
		if (asset_->IsScenePreloadFinished(nextSceneType_)) {
			if (!needInitNextScene_) {

				LoadScene(nextSceneType_);
				needInitNextScene_ = true;
			}
		} else {

			// Loading中の処理はここに入れる
		}
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

bool SceneManager::ConsumeNeedInitNextScene() {

	bool need = needInitNextScene_;
	needInitNextScene_ = false;
	return need;
}

void SceneManager::LoadScene(Scene scene) {

	currentScene_.reset();
	// 次のSceneを作成
	currentSceneType_ = scene;
	currentScene_ = factory_->Create(scene);
	currentScene_->SetPtr(postProcessSystem_, sceneView_, this);

	// imgui選択をリセット
	ImGuiObjectEditor::GetInstance()->Reset();
	// すべてのオブジェクトを破棄
	ObjectManager::GetInstance()->DestroyAll();
}