#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Base/BaseCamera.h>
#include <Game/Camera/DebugCamera.h>
#include <Game/Camera/LightViewCamera.h>
#include <Game/Camera/Camera2D.h>

// c++
#include <memory>
#include <optional>

//============================================================================
//	CameraManager class
//============================================================================
class CameraManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	CameraManager() = default;
	~CameraManager() = default;

	void Init();

	void Update();

	//--------- accessor -----------------------------------------------------

	void SetCamera(BaseCamera* gameCamera);

	BaseCamera* GetCamera() const;

	DebugCamera* GetDebugCamera() const { return debugCamera_.get(); }

	LightViewCamera* GetLightViewCamera() const { return lightViewCamera_.get(); }

	Camera2D* GetCamera2D() const { return camera2D_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::optional<BaseCamera*> gameCamera_;

	std::unique_ptr<DebugCamera> debugCamera_;

	std::unique_ptr<LightViewCamera> lightViewCamera_;

	std::unique_ptr<Camera2D> camera2D_;

	//--------- functions ----------------------------------------------------

	void RenderCameraFrame();
};