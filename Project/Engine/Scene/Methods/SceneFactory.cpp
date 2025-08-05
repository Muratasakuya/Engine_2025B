#include "SceneFactory.h"

//============================================================================
//	include
//============================================================================
#include <Game/Scene/EffectScene.h>
#include <Game/Scene/TitleScene.h>
#include <Game/Scene/GameScene.h>
#include <Game/Scene/ClearScene.h>

//============================================================================
//	SceneFactory classMethods
//============================================================================

std::unique_ptr<IScene> SceneFactory::Create(Scene scene) {

	switch (scene) {
	case Scene::Effect:

		return std::make_unique<EffectScene>();
	case Scene::Title:

		return std::make_unique<TitleScene>();
	case Scene::Game:

		return std::make_unique<GameScene>();
	case Scene::Clear:

		return std::make_unique<ClearScene>();
	}
	return nullptr;
}