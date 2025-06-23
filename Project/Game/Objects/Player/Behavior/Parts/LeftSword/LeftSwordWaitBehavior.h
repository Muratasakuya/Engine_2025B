#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftSwordWaitBehavior class
//============================================================================
class LeftSwordWaitBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftSwordWaitBehavior(const Json& data, const Quaternion& initRotation);
	~LeftSwordWaitBehavior() = default;

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

	// 初期化時の回転角
	Quaternion initRotation_;

	// parameter
	// 回転補間割合
	float rotationLerpRate_;
};
