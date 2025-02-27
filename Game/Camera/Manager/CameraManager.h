#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Base/BaseCamera.h>
#include <Game/Camera/LightViewCamera.h>

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

	LightViewCamera* GetLightViewCamera() const { return lightViewCamera_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::optional<BaseCamera*> gameCamera_;

	std::unique_ptr<LightViewCamera> lightViewCamera_;
};