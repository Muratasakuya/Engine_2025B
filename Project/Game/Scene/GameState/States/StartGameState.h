#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/GameState/Interface/IGameSceneState.h>

//============================================================================
//	StartGameState class
//============================================================================
class StartGameState :
	public IGameSceneState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	StartGameState(GameContext* context) :IGameSceneState(context) {}
	~StartGameState() = default;

	void Init(SceneView* sceneView) override;

	void Update(SceneManager* sceneManager) override;
	void NonActiveUpdate(SceneManager* sceneManager) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};