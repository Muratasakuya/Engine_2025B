#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftHandThirdAttackBehavior class
//============================================================================
class LeftHandThirdAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftHandThirdAttackBehavior(const Json& data);
	~LeftHandThirdAttackBehavior() = default;

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

	// 回転攻撃が始まるまでの時間
	float moveWaitTimer_;
	float moveWaitTime_;
	bool enableMove_; // 動き出し可能

	// 回転
	// 初期回転
	Vector3 initRotationAngle_;
	bool setRotation_; // 値を設定したか

	//--------- functions ----------------------------------------------------

	void SetStartRotation(BasePlayerParts* parts);

	void WaitAttackTime();
};
