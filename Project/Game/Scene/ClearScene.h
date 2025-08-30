#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Methods/IScene.h>

// scene
#include <Engine/Scene/Light/PunctualLight.h>
#include <Game/Camera/ResultCamera/ClearResultCamera.h>
#include <Game/Objects/SceneTransition/FadeTransition.h>

//============================================================================
//	ClearScene class
//============================================================================
class ClearScene :
	public IScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ClearScene() = default;
	~ClearScene() = default;

	void Init() override;

	void Update() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// scene
	std::unique_ptr<ClearResultCamera> resultCamera_;
	std::unique_ptr<PunctualLight> light_;
	std::unique_ptr<FadeTransition> fadeTransition_;
};