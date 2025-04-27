#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodyWalkBehavior class
//============================================================================
class BodyWalkBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodyWalkBehavior(const Json& data, FollowCamera* followCamera);
	~BodyWalkBehavior() = default;

	// 処理実行
	void Execute(BasePlayerParts* parts) override;
	// リセット
	void Reset();

	// imgui
	void ImGui() override;

	// json
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	float GetSpeed() const { return speed_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 move_; // 移動量

	// parameter
	// 歩き速度
	float speed_;
	// 速度減衰率
	float moveDecay_;
	// 回転補間割合
	float rotationLerpRate_;

	//--------- functions ----------------------------------------------------

	void UpdateWalk(BasePlayerParts* parts);
	void RotateToDirection(BasePlayerParts* parts);
};