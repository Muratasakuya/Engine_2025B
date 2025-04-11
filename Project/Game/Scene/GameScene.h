#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Light/PunctualLight.h>
#include <Game/Scene/Methods/IScene.h>
#include <Game/Camera/GameCamera.h>

// editor
#include <Game/Editor/Object/Object3DEditor.h>

// object

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
		LightManager* lightManager, PostProcessSystem* postProcessSystem) override;

	void Update(SceneManager* sceneManager) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// editor
	std::unique_ptr<Object3DEditor> object3DEditor_;

	// camera
	std::unique_ptr<GameCamera> gameCamera_;
	// light
	std::unique_ptr<PunctualLight> gameLight_;
};