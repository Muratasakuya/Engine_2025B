#include "IScene.h"

//============================================================================
//	IScene classMethods
//============================================================================

IScene::IScene(Asset* asset, PostProcessSystem* postProcessSystem,
	SceneView* sceneView, SceneManager* sceneManager) {

	asset_ = nullptr;
	asset_ = asset;

	postProcessSystem_ = nullptr;
	postProcessSystem_ = postProcessSystem;

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	sceneManager_ = nullptr;
	sceneManager_ = sceneManager;
}