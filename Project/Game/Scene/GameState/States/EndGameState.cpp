#include "EndGameState.h"

//============================================================================
//	EndGameState classMethods
//============================================================================

void EndGameState::Init([[maybe_unused]] SceneView* sceneView) {
}

void EndGameState::Update([[maybe_unused]] SceneManager* sceneManager) {

	const GameSceneState currentState = GameSceneState::EndGame;

	//========================================================================
	//	scene
	//========================================================================

	context_->camera->Update(currentState);

	//========================================================================
	//	sceneEvent
	//========================================================================

	// カメラの演出が終了次第クリアシーンに移す
	if (context_->camera->GetEndGameCamera()->IsFinished()) {

		requestNext_ = true;
	}
}

void EndGameState::NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {
}