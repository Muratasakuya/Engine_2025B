#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Methods/IScene.h>

// scene
#include <Game/Camera/Manager/CameraManager.h>
#include <Engine/Scene/Light/PunctualLight.h>

// object
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

// effect
#include <Engine/Effect/GameEffect.h>

// editor
#include <Game/Editor/GameObjectEditor.h>
#include <Game/Editor/LevelEditor/LevelEditor.h>

//============================================================================
//	GameScene class
//============================================================================
class GameScene :
	public IScene, public IGameEditor {
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

	// camera
	std::unique_ptr<CameraManager> cameraManager_;
	// light
	std::unique_ptr<PunctualLight> gameLight_;

	// objects
	std::unique_ptr<Player> player_;
	std::unique_ptr<BossEnemy> bossEnemy_;

	// effects
	std::unique_ptr<GameEffect> demoEffect_;

	// editor
	std::unique_ptr<GameObjectEditor> objectEditor_;
	std::unique_ptr<LevelEditor> levelEditor_;

	//--------- functions ----------------------------------------------------

	void Load();
};