#pragma once

//============================================================================
//  include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//  RightSwordThirdAttackBehavior class
//============================================================================
class RightSwordThirdAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//  public Methods
	//========================================================================

	RightSwordThirdAttackBehavior(const Json& data, const Vector3& initOffsetTranslate);
	~RightSwordThirdAttackBehavior() = default;

	// 処理実行
	void Execute(BasePlayerParts* parts) override;
	// リセット
	void Reset();

	// imgui
	void ImGui() override;

	// json
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	void SetForwardDirection(const Vector3& direction);
private:
	//========================================================================
	//  private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 最初の前方ベクトル
	Vector3 forwardDirection_;
	// 移動量
	float moveValue_;
	// 下がった後に前に行くまでの時間
	float moveWaitTimer_;
	float moveWaitTime_;
	bool enableMoveFront_; // 動き出し可能

	// 移動
	// 向き
	Vector3 axis_;
	float rotateAngle_;
	// プレイヤーの前方に移動させる
	std::unique_ptr<SimpleAnimation<Vector3>> moveForward_;
	// 元の手の場所に戻す
	std::unique_ptr<SimpleAnimation<Vector3>> returnHand_;

	//--------- functions ----------------------------------------------------

	void UpdateMoveForward(BasePlayerParts* parts);

	void UpdateReturnHand(BasePlayerParts* parts);

	void WaitMoveTime();
};