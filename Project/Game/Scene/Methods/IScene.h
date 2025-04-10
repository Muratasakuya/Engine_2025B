#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Base/IComponent.h>

// c++
#include <string>
// front
class Asset;
class CameraManager;
class SceneManager;
class PostProcessSystem;

//============================================================================
//	Scene
//============================================================================

enum class Scene {

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
		PostProcessSystem* postProcessSystem) = 0;

	virtual void Update(SceneManager* sceneManager) = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	CameraManager* cameraManager_;
	PostProcessSystem* postProcessSystem_;
};