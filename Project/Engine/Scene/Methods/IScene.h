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

	Effect,
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

	virtual void Init() = 0;

	virtual void Update() = 0;

	//--------- accessor -----------------------------------------------------

	void SetPtr(Asset* asset, PostProcessSystem* postProcessSystem,
		SceneView* sceneView, SceneManager* sceneManager);
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