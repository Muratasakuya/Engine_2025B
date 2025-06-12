#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>

// c++
#include <memory>

//============================================================================
//	SceneFactory class
//============================================================================
class SceneFactory {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneFactory() = default;
	~SceneFactory() = default;

	std::unique_ptr<IScene> Create(Scene scene, Asset* asset, PostProcessSystem* postProcessSystem,
		SceneView* sceneView, SceneManager* sceneManager);
};