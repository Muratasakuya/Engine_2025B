#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightSwordWalkBehavior class
//============================================================================
class RightSwordWalkBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightSwordWalkBehavior(const Json& data, const Quaternion& initRotation);
	~RightSwordWalkBehavior() = default;

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
