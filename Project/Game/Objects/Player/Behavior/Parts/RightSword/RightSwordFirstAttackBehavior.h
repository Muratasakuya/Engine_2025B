#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightSwordFirstAttackBehavior class
//============================================================================
class RightSwordFirstAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightSwordFirstAttackBehavior(const Json& data);
	~RightSwordFirstAttackBehavior() = default;

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

	// 剣の向きを振る方向に合わせる
	std::unique_ptr<SimpleAnimation<Vector3>> rotationLerpValue_;
};
