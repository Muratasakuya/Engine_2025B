#include "SceneFactory.h"

//============================================================================
//	include
//============================================================================
#include <Game/Scene/DebugScene/DebugScene.h>

//============================================================================
//	SceneFactory classMethods
//============================================================================

std::unique_ptr<IScene> SceneFactory::Create(Scene scene) {

	switch (scene) {
	case Scene::Debug:

		return std::make_unique<DebugScene>();
	case Scene::Title:

		return nullptr;
	case Scene::Game:

		return nullptr;
	default:

		return nullptr;
	}
}