#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightHandWaitBehavior class
//============================================================================
class RightHandWaitBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightHandWaitBehavior(const std::optional<Json>& data,const Vector3& initRotationAngle);
	~RightHandWaitBehavior() = default;

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

	// Execute処理に入った瞬間の回転角、lerpの初期値にする
	std::optional<Quaternion> startRotation_;

	// parameter
	// 目標回転角、初期化時の値を終わりに入れる
	std::unique_ptr<SimpleAnimation<Vector3>> rotationLerpValue_;
};