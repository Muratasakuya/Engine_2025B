#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/GameState/Interface/IGameSceneState.h>

//============================================================================
//	PlayGameState class
//============================================================================
class PlayGameState :
	public IGameSceneState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayGameState(GameContext* context) :IGameSceneState(context) {}
	~PlayGameState() = default;

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