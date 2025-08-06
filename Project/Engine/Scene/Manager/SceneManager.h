#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Methods/IScene.h>
#include <Engine/Scene/Methods/SceneFactory.h>
#include <Engine/Scene/Methods/SceneTransition.h>
#include <Engine/Editor/Base/IGameEditor.h>

// c++
#include <memory>
// front
class Asset;

//============================================================================
//	SceneManager class
//============================================================================
class SceneManager :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneManager(Scene scene, Asset* asset, SceneView* sceneView);
	~SceneManager() = default;

	void Update();

	void SwitchScene();

	void InitNextScene();

	void SetNextScene(Scene scene, std::unique_ptr<ITransition> transition);

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	bool IsSceneSwitching() const { return isSceneSwitching_; }

	bool ConsumeNeedInitNextScene();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	SceneView* sceneView_;

	std::unique_ptr<IScene> currentScene_;

	std::unique_ptr<SceneFactory> factory_;

	std::unique_ptr<SceneTransition> sceneTransition_;

	Scene currentSceneType_;
	Scene nextSceneType_;
	bool isSceneSwitching_;
	bool needInitNextScene_;

	//--------- functions ----------------------------------------------------

	void LoadScene(Scene scene);
};