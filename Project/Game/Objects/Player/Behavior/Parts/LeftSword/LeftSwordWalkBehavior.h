#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftSwordWalkBehavior class
//============================================================================
class LeftSwordWalkBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftSwordWalkBehavior(const Json& data, const Quaternion& initRotation);
	~LeftSwordWalkBehavior() = default;

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
