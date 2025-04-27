#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodyFirstAttackBehavior class
//============================================================================
class BodyFirstAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodyFirstAttackBehavior(const Json& data);
	~BodyFirstAttackBehavior() = default;

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

	bool setStart_; // 初期座標を設定したかどうか

	// parameter
	// 進む量
	float moveValue_;
	// 向いている方向に向かって直進させる
	std::unique_ptr<SimpleAnimation<Vector3>> moveStraight_;
};