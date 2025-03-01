#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Camera/GameCamera.h>
#include <Game/Base/BaseGameObject.h>

// c++
#include <memory>
#include <vector>

//============================================================================
//	DebugScene class
//============================================================================
class DebugScene :
	public IScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	DebugScene() = default;
	~DebugScene() = default;

	void Init(Asset* asset,CameraManager* cameraManager) override;

	void Update(SceneManager* sceneManager) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<GameCamera> gameCamera_;

	std::vector<std::unique_ptr<BaseGameObject>> objects_;

	//--------- functions ----------------------------------------------------

};