#include "PlayGameState.h"

//============================================================================
//	PlayGameState classMethods
//============================================================================

void PlayGameState::Init([[maybe_unused]] SceneView* sceneView) {
}

void PlayGameState::Update([[maybe_unused]] SceneManager* sceneManager) {

	const GameSceneState currentState = GameSceneState::PlayGame;

	//========================================================================
	//	object
	//========================================================================

	context_->player->Update();
	context_->boss->Update(currentState);

	//========================================================================
	//	sprite
	//========================================================================

	// 遷移後処理
	context_->fadeSprite->Update();

	//========================================================================
	//	scene
	//========================================================================

	context_->camera->Update(currentState);
	context_->level->Update();
}

void PlayGameState::NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {
}