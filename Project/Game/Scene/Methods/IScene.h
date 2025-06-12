#pragma once

//============================================================================
//	include
//============================================================================

// front
class Asset;
class SceneView;
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

	IScene(Asset* asset, PostProcessSystem* postProcessSystem,
		SceneView* sceneView, SceneManager* sceneManager);
	virtual ~IScene() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;
	PostProcessSystem* postProcessSystem_;

	SceneView* sceneView_;
	SceneManager* sceneManager_;
};