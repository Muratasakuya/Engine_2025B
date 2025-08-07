#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Methods/IScene.h>

// controller
#include <Game/Objects/TitleScene/TitleController.h>

// scene
#include <Engine/Scene/Camera/BaseCamera.h>
#include <Engine/Scene/Light/PunctualLight.h>

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
	~TitleScene() = default;

	void Init() override;

	void Update() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// controller
	std::unique_ptr<TitleController> controller_;

	// scene
	std::unique_ptr<BaseCamera> camera3D_;
	std::unique_ptr<PunctualLight> light_;
};