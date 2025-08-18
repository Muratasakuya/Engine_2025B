#include "BeginGameState.h"

//============================================================================
//	BeginGameState classMethods
//============================================================================

void BeginGameState::Init([[maybe_unused]] SceneView* sceneView) {
}

void BeginGameState::Update([[maybe_unused]] SceneManager* sceneManager) {

	const GameSceneState currentState = GameSceneState::BeginGame;

	//========================================================================
	//	object
	//========================================================================

	context_->player->Update();
	context_->boss->Update(currentState);

	//========================================================================
	//	scene
	//========================================================================

	context_->camera->Update(currentState);
	context_->level->Update();

	//========================================================================
	//	sceneEvent
	//========================================================================

	// ボスの登場演出が終了したらゲーム開始
}

void BeginGameState::NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {


}

void BeginGameState::ImGui() {


}