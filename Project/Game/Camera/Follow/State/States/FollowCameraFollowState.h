#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/State/Interface/FollowCameraIState.h>

//============================================================================
//	FollowCameraFollowState class
//============================================================================
class FollowCameraFollowState :
	public FollowCameraIState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraFollowState(float defaultFovY) :defaultFovY_(defaultFovY) {}
	~FollowCameraFollowState() = default;

	void Enter() override;

	void Update(FollowCamera& followCamera)  override;

	void Exit()  override;

	// imgui
	void ImGui(const FollowCamera& followCamera) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data)  override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct RotateParam {

		float rotateClampX;   // 範囲制限
		float offsetZNear;    // 制限値までの距離
		float clampThreshold; // 補間閾値
	};

	//--------- variables ----------------------------------------------------

	Vector2 smoothedInput_; // 入力の値補間用
	float defaultFovY_;

	// parameters
	Vector3 offsetTranslation_; // 追従相手との距離
	Vector3 interTarget_;       // 追従中間target位置

	float lerpRate_;           // 補間割合
	float inputLerpRate_;      // 入力補間割合
	Vector2 mouseSensitivity_; // マウス感度
	Vector2 padSensitivity_;   // パッド操作の感度

	// 回転の設定
	float defaultOffsetZ_;         // z初期値
	float offsetZLerpRate_;        // z値補間割合
	RotateParam rotatePlusParam_;  // +
	RotateParam rotateMinusParam_; // -
};