#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/Camera/BaseCamera.h>

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

	void SetTarget(const Transform3DComponent& target) { target_ = &target; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const Transform3DComponent* target_;
	float aspectRatio_;

	// parameter
	Vector3 offsetTranslation_; // 追従相手との距離
	Vector3 interTarget_;       // 追従中間target位置

	float lerpRate_; // 補間速度

	//--------- functions ----------------------------------------------------

	// init
	void InitParam();

	// update
	void Move();
	void UpdateMatrix();

	// json
	void ApplyJson();
	void SaveJson();
};