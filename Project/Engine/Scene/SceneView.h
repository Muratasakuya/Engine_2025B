#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// buffer
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>

// camera
#include <Engine/Scene/Camera/BaseCamera.h>
#include <Engine/Scene/Camera/DebugCamera.h>
#include <Engine/Scene/Camera/Camera2D.h>

// light
#include <Engine/Scene/Light/PunctualLight.h>

// c++
#include <memory>
#include <optional>

//============================================================================
//	SceneView class
//============================================================================
class SceneView :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneView() : IGameEditor("SceneView") {};
	~SceneView() = default;

	void Init();

	void Update();

	void ImGui() override;
	//--------- accessor -----------------------------------------------------

	// camera
	void SetCamera(BaseCamera* gameCamera);

	BaseCamera* GetCamera() const { return activeCamera3D_.value(); }
	DebugCamera* GetDebugCamera() const { return debugCamera_.get(); }
	Camera2D* GetCamera2D() const { return camera2D_.get(); }

	// light
	void SetLight(PunctualLight* gameLight);

	PunctualLight* GetLight() const { return punctualLight_.value(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// camera
	// 3Dシーン
	std::optional<BaseCamera*> activeCamera3D_; // 使用されているカメラ
	std::unique_ptr<DebugCamera> debugCamera_;  // デバッグ確認用
	// 2Dシーン
	std::unique_ptr<Camera2D> camera2D_;

	// light
	std::optional<PunctualLight*> punctualLight_;

	//--------- functions ----------------------------------------------------

	// init
	void InitCamera();

	// update
	void UpdateCamera();
	void UpdateLight();

	void EditCamera();
	void EditLight();

	// debug
	void DisplayPointLight();
	void DisplaySpotLight();
};