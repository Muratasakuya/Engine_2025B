#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <string>
// front
class Asset;
class CameraManager;
class SceneManager;
class PostProcessManager;

//============================================================================
//	Scene
//============================================================================

enum class Scene {

	Debug,
	Title,
	Game
};

//============================================================================
//	IScene class
//============================================================================
class IScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IScene() = default;
	virtual ~IScene() = default;

	virtual void Init(Asset* asset, CameraManager* cameraManager,
		PostProcessManager* postProcessManager) = 0;

	virtual void Update(SceneManager* sceneManager) = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	CameraManager* cameraManager_;
	PostProcessManager* postProcessManager_;
};