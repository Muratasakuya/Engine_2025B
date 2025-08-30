#include "ResultGameState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>

//============================================================================
//	ResultGameState classMethods
//============================================================================

void ResultGameState::Enter() {

	//========================================================================
	//	postProcess
	//========================================================================

	PostProcessSystem* postProcessSystem = PostProcessSystem::GetInstance();
	postProcessSystem->ClearProcess();
	postProcessSystem->AddProcess(PostProcessType::Grayscale);
}

void ResultGameState::Init([[maybe_unused]] SceneView* sceneView) {
}

void ResultGameState::Update([[maybe_unused]] SceneManager* sceneManager) {

	const GameSceneState currentState = GameSceneState::Result;

	//========================================================================
	//	scene
	//========================================================================

	context_->camera->Update(currentState);

	//========================================================================
	//	sceneEvent
	//========================================================================

	// 入力に応じて遷移先を決定する
}

void ResultGameState::NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {
}