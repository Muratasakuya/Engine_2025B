#include "IScene.h"

//============================================================================
//	IScene classMethods
//============================================================================

void IScene::SetPtr(PostProcessSystem* postProcessSystem,
	SceneView* sceneView, SceneManager* sceneManager) {

	postProcessSystem_ = nullptr;
	postProcessSystem_ = postProcessSystem;

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	sceneManager_ = nullptr;
	sceneManager_ = sceneManager;
}