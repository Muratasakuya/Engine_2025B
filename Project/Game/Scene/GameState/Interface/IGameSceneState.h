#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/GameState/GameContext.h>
#include <Game/Scene/GameState/GameSceneState.h>

// front
class SceneManager;

//============================================================================
//	IGameSceneState class
//============================================================================
class IGameSceneState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IGameSceneState(GameContext* context) :context_(context) {}
	virtual ~IGameSceneState() = default;

	// 初期化
	virtual void Init(SceneView* sceneView) = 0;

	// 更新処理
	virtual void Update(SceneManager* sceneManager) = 0;
	virtual void NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {}

	// 遷移開始時
	virtual void Enter() {}
	// 遷移終了時
	virtual void Exit() {}

	// エディター
	virtual void ImGui() {}

	//--------- accessor -----------------------------------------------------

	bool IsRequestNext() const { return requestNext_; }
	void ClearRequestNext() { requestNext_ = false; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	GameContext* context_;

	// 次の状態への遷移
	bool requestNext_ = false;
};