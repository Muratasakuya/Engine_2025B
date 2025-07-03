#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Camera/BaseCamera.h>

// state
#include <Game/Camera/Follow/State/FollowCameraStateController.h>

//============================================================================
//	FollowCamera class
//============================================================================
class FollowCamera :
	public BaseCamera {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCamera() = default;
	~FollowCamera() = default;

	void Init() override;

	void Update() override;

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	void StartScreenShake(bool isShake);
	void SetTarget(const Transform3DComponent& target);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------
	
	// 状態の管理
	std::unique_ptr<FollowCameraStateController> stateController_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdateMatrix();

	// json
	void ApplyJson();
	void SaveJson();
};