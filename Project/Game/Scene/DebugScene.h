#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Scene/Methods/IScene.h>
#include <Game/Camera/GameCamera.h>

// object
#include <Game/Object3D/TemplateObject3D.h>

// editor
#include <Game/Editor/TestEditor.h>

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

	std::unique_ptr<TemplateObject3D> object_;

	std::unique_ptr<TestEditor> testEditor_;

	//--------- functions ----------------------------------------------------

};