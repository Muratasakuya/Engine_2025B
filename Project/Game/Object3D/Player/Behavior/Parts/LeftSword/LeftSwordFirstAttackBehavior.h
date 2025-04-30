#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftSwordFirstAttackBehavior class
//============================================================================
class LeftSwordFirstAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftSwordFirstAttackBehavior(const Json& data);
	~LeftSwordFirstAttackBehavior() = default;

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

	// 開始時の回転角
	Quaternion startRotation_;
	// 剣の向きを振る方向に合わせる
	std::unique_ptr<SimpleAnimation<Vector3>> rotationLerpValue_;
};