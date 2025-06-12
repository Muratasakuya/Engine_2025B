#include "SceneFactory.h"

//============================================================================
//	include
//============================================================================
#include <Game/Scene/TitleScene.h>
#include <Game/Scene/GameScene.h>

//============================================================================
//	SceneFactory classMethods
//============================================================================

std::unique_ptr<IScene> SceneFactory::Create(Scene scene, Asset* asset,
	PostProcessSystem* postProcessSystem, SceneView* sceneView, SceneManager* sceneManager) {

	switch (scene) {
	case Scene::Title:

		return std::make_unique<TitleScene>(asset, postProcessSystem,
			sceneView, sceneManager);
	case Scene::Game:

		return std::make_unique<GameScene>(asset, postProcessSystem,
			sceneView, sceneManager);
	}

	return nullptr;
}