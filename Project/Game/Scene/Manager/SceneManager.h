#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Scene/Methods/SceneFactory.h>
#include <Game/Scene/Methods/SceneTransition.h>

// c++
#include <memory>

//============================================================================
//	SceneManager class
//============================================================================
class SceneManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneManager(Scene scene, Asset* asset,
		PostProcessSystem* postProcessSystem, SceneView* sceneView);
	~SceneManager() = default;

	void Update();

	void SwitchScene();

	void InitNextScene();

	void SetNextScene(Scene scene, std::unique_ptr<ITransition> transition);

	//--------- accessor -----------------------------------------------------

	bool IsSceneSwitching() const { return isSceneSwitching_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	PostProcessSystem* postProcessSystem_;
	SceneView* sceneView_;

	std::unique_ptr<IScene> currentScene_;

	std::unique_ptr<SceneFactory> factory_;

	std::unique_ptr<SceneTransition> sceneTransition_;

	Scene nextScene_;
	bool isSceneSwitching_;

	//--------- functions ----------------------------------------------------

	void LoadScene(Scene scene);
};