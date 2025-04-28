#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightHandFirstAttackBehavior class
//============================================================================
class RightHandFirstAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightHandFirstAttackBehavior(const Json& data);
	~RightHandFirstAttackBehavior() = default;

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
	//  private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 回転角
	Vector3 rotationAngle_;
	// 開始時の回転角
	Quaternion startRotation_;
	// 左後ろに回転
	std::unique_ptr<SimpleAnimation<Vector3>> backLeftRotation_;
	// 左後ろに回転したあと前に斬りかかる
	std::unique_ptr<SimpleAnimation<Vector3>> forwardSlash_;

	//--------- functions ----------------------------------------------------

	void UpdateBackLeftRotation(BasePlayerParts* parts);
	void UpdateForwardSlash(BasePlayerParts* parts);
};