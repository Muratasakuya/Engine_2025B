#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

// front
class FollowCamera;

//============================================================================
//	PlayerBody class
//============================================================================
class PlayerBody :
	public BasePlayerParts {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerBody() = default;
	~PlayerBody() = default;

	void Init(FollowCamera* followCamera);

	// 歩き処理
	void UpdateWalk();
	// 向きの更新
	void RotateToDirection();

	void ImGui();

	// json
	void SaveJson();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	FollowCamera* followCamera_;

	Vector3 move_; // 移動量

	// parameter
	Vector3 moveVelocity_;   // 移動速度
	float moveDecay_;        // 速度減衰率
	float rotationLerpRate_; // 回転補間割合

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();

	// init
	void InitBehaviors();
	void InitBehaviors(const Json& data);
};