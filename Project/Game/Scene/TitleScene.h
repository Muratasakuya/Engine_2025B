#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Camera/GameCamera.h>

// object
#include <Game/Object2D/SceneTransition/FadeTransition.h>

//============================================================================
//	TitleScene class
//============================================================================
class TitleScene :
	public IScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TitleScene() = default;
	~TitleScene();

	void Init(Asset* asset, CameraManager* cameraManager,
		PostProcessManager* postProcessManager) override;

	void Update(SceneManager* sceneManager) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<GameCamera> gameCamera_;

	std::unique_ptr<FadeTransition> fadeTransition_;

	EntityID titleNameId_;
};