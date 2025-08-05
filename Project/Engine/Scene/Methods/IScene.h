#pragma once

//============================================================================
//	include
//============================================================================

// front
class SceneView;
class SceneManager;
class PostProcessSystem;

//============================================================================
//	Scene
//============================================================================

enum class Scene {

	Effect,
	Title,
	Game,
	Clear
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

	void SetPtr(PostProcessSystem* postProcessSystem,
		SceneView* sceneView, SceneManager* sceneManager);
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	PostProcessSystem* postProcessSystem_;

	SceneView* sceneView_;
	SceneManager* sceneManager_;
};