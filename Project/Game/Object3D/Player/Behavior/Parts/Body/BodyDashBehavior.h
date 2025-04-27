#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodyDashBehavior class
//============================================================================
class BodyDashBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodyDashBehavior(const Json& data, FollowCamera* followCamera);
	~BodyDashBehavior() = default;

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
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Vector3 move_; // 移動量

	// parameter
	// ダッシュ速度
	float speed_;
	// ダッシュ補間
	std::unique_ptr<SimpleAnimation<float>> speedLerpValue_;
};