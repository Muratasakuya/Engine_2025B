#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Methods/IScene.h>

// scene
#include <Game/Scene/GameState/Interface/IGameSceneState.h>

// effect
#include <Engine/Effect/GameEffect.h>

//============================================================================
//	GameScene class
//============================================================================
class GameScene :
	public IScene,
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameScene() :IGameEditor("GameScene") {};
	~GameScene() = default;

	void Init() override;

	void Update() override;

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// sceneState
	GameSceneState currentState_;
	GameContext context_;
	std::array<std::unique_ptr<IGameSceneState>, static_cast<uint32_t>(GameSceneState::Count)> states_;

	// camera
	std::unique_ptr<CameraManager> cameraManager_;
	// light
	std::unique_ptr<PunctualLight> gameLight_;
	// collision
	std::unique_ptr<FieldBoundary> fieldBoundary_;

	// objects
	std::unique_ptr<Player> player_;
	std::unique_ptr<BossEnemy> bossEnemy_;

	// editor
	std::unique_ptr<LevelEditor> levelEditor_;

	//--------- functions ----------------------------------------------------

	// init
	void InitStates();

	// helper
	void RequestNextState(GameSceneState next);
};