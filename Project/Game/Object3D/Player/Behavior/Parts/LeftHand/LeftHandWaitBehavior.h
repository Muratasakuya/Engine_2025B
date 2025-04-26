#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  LeftHandWaitBehavior class
//============================================================================
class LeftHandWaitBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	LeftHandWaitBehavior(const std::optional<Json>& data, const Vector3& initRotationAngle);
	~LeftHandWaitBehavior() = default;

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
