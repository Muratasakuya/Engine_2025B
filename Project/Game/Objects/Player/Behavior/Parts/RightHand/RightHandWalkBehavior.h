#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightHandWalkBehavior class
//============================================================================
class RightHandWalkBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightHandWalkBehavior(const Json& data);
	~RightHandWalkBehavior() = default;

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

	// 親の回転角
	Quaternion parentRotation_;

	// parameter
	// 回転補間割合
	float rotationLerpRate_;
};
