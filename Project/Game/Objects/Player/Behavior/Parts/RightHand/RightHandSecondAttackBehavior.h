#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightHandSecondAttackBehavior class
//============================================================================
class RightHandSecondAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightHandSecondAttackBehavior(const Json& data);
	~RightHandSecondAttackBehavior() = default;

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

	// 回転
	// 回転角
	Vector3 rotationAngle_;
	// 開始時の回転角
	Quaternion startRotation_;
	// 左前に行くとき
	std::unique_ptr<SimpleAnimation<Vector3>> frontLeftRotation_;
	// 左前行った後に右前に行くとき、ここが一番移動量多め
	std::unique_ptr<SimpleAnimation<Vector3>> frontRightRotation_;
	// 右前行った後に左前に行って正面に戻る
	std::unique_ptr<SimpleAnimation<Vector3>> returnCenterRotation_;

	//--------- functions ----------------------------------------------------

	void UpdateFrontLeftRotation(BasePlayerParts* parts);
	void UpdateFrontRightRotation(BasePlayerParts* parts);
	void UpdateReturnCenterRotation(BasePlayerParts* parts);
};