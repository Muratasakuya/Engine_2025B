#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Camera/GameCamera.h>

// object
#include <Game/Object3D/Environment/StageField.h>

//============================================================================
//	GameScene class
//============================================================================
class GameScene :
	public IScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameScene() = default;
	~GameScene() = default;

	void Init(Asset* asset, CameraManager* cameraManager,
		PostProcessManager* postProcessManager) override;

	void Update(SceneManager* sceneManager) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<GameCamera> gameCamera_;

	std::unique_ptr<StageField> stageField_;
};