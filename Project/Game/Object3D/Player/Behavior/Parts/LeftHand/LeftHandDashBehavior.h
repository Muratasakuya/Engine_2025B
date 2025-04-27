#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftHandDashBehavior class
//============================================================================
class LeftHandDashBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LeftHandDashBehavior(const Json& data);
	~LeftHandDashBehavior() = default;

	// 処理実行
	void Execute(BasePlayerParts* parts) override;
	// リセット
	void Reset();

	// imgui
	void ImGui() override;

	// json
	void SaveJson(Json& data) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// Execute処理に入った瞬間の回転角、lerpの初期値にする
	std::optional<Quaternion> startRotation_;

	// parameter
	// 回転角
	Vector3 rotationAngle_;
	// 目標回転角
	std::unique_ptr<SimpleAnimation<Vector3>> rotationLerpValue_;
};