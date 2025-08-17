#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/GameState/Interface/IGameSceneState.h>

//============================================================================
//	BeginGameState class
//============================================================================
class BeginGameState :
	public IGameSceneState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BeginGameState(GameContext* context) :IGameSceneState(context){}
	~BeginGameState() = default;

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