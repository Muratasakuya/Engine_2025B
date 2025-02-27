#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Scene/Methods/SceneFactory.h>

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

	SceneManager(Scene scene, Asset* asset, CameraManager* cameraManager);
	~SceneManager() = default;

	void Update();

	void SwitchScene();

	void InitNextScene();

	void SetNextScene(Scene scene);

	//--------- accessor -----------------------------------------------------

	bool IsSceneSwitching() const { return isSceneSwitching_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	CameraManager* cameraManager_;

	std::unique_ptr<IScene> currentScene_;

	std::unique_ptr<SceneFactory> factory_;

	Scene nextScene_;
	bool isSceneSwitching_;

	//--------- functions ----------------------------------------------------

	void LoadScene(Scene scene);
};