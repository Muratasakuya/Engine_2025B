#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Input/InputStructures.h>
#include <Engine/Scene/Camera/BaseCamera.h>
#include <Lib/Adapter/Easing.h>

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
	void FirstUpdate();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	void StartScreenShake(bool isShake) { isScreenShake_ = isShake; }

	void SetTarget(const Transform3DComponent& target) { target_ = &target; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 現在の入力状態
	InputType inputType_;

	const Transform3DComponent* target_; // 追従対象

	// parameter
	Vector3 offsetTranslation_;    // 追従相手との距離
	Vector3 interTarget_;          // 追従中間target位置
	float eulerRotateClampPlusX_;  // カメラのX軸回転の制限: +
	float eulerRotateClampMinusX_; // カメラのX軸回転の制限: -

	float lerpRate_;         // 補間速度
	Vector2 sensitivity_;    // マウス感度
	Vector2 padSensitivity_; // パッド操作の感度

	bool isScreenShake_;                // 画面シェイク中かどうか
	float screenShakeXZIntensity_;      // 画面シェイクのXZ強度
	float screenShakeOffsetYIntensity_; // 画面シェイクのオフセットY強度
	float screenShakeDuration_;         // 画面シェイクの持続時間
	float screenShakeTimer_;            // シェイクの経過時間
	EasingType screenShakeEasingType_;  // シェイクのイージングタイプ
	
	// editor
	bool isDebugMode_;

	//--------- functions ----------------------------------------------------

	// init
	void InitParam();

	// update
	void Move();
	void UpdateMatrix();

	void UpdateScreenShake();

	// json
	void ApplyJson();
	void SaveJson();
};