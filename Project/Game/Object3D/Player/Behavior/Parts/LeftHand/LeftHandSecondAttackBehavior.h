#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftHandSecondAttackBehavior class
//============================================================================
class LeftHandSecondAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftHandSecondAttackBehavior(const Json& data);
	~LeftHandSecondAttackBehavior() = default;

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



	//--------- functions ----------------------------------------------------

	void UpdateFrontLeftStep(BasePlayerParts* parts);
	void UpdateFrontRightStep(BasePlayerParts* parts);
	void UpdateReturnCenterStep(BasePlayerParts* parts);
};
