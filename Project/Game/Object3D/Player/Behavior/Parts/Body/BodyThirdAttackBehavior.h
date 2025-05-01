#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodyThirdAttackBehavior class
//============================================================================
class BodyThirdAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodyThirdAttackBehavior(const Json& data, FollowCamera* followCamera);
	~BodyThirdAttackBehavior() = default;

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

	// 最初の後方ベクトル
	Vector3 backwardDirection_;

	// parameter
	// 移動
	// 進む量
	float moveValue_;
	// 下がった後に前に行くまでの時間
	float moveWaitTimer_;
	float moveWaitTime_;
	bool enableMoveFront_; // 動き出し可能
	// 後ろに下がっていくとき
	std::unique_ptr<SimpleAnimation<Vector3>> moveBack_;
	// 下がった後、数フレーム後に前に行くとき
	std::unique_ptr<SimpleAnimation<Vector3>> moveFront_;

	//--------- functions ----------------------------------------------------

	void UpdateMoveBack(BasePlayerParts* parts);
	void UpdateMoveFront(BasePlayerParts* parts);

	void WaitMoveTime();
};