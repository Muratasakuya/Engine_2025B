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

	// catmullRom、座標操作
	void EditCatmullRom(const Vector3& translation,
		const Vector3& direction);

	//--------- accessor -----------------------------------------------------

	void SetBackwardDirection(const Vector3& direction);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 最初の後方ベクトル
	Vector3 backwardDirection_;
	// 最初の前方ベクトル
	Vector3 forwardDirection_;

	// -------- 前半攻撃処理 --------//
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

	// 回転
	// 回転角
	Vector3 rotationAngle_;
	// 開始時の回転角
	Quaternion startRotation_;
	// 後ろに下がっていくとき、下がりきったときには1回転させる
	std::unique_ptr<SimpleAnimation<Vector3>> backRotation_;
	// 前に行くとき、正面に突進するような感じ
	std::unique_ptr<SimpleAnimation<Vector3>> frontRotation_;

	// -------- 後半攻撃処理 --------//
	// 移動
	// catmullRom曲線移動に使うkeyframe
	// 後方ベクトル方向を常に向くようにする
	std::vector<Vector3> moveKeyframes_;
	std::unique_ptr<SimpleAnimation<Vector3>> moveKeyframeAnimation_;

	//--------- functions ----------------------------------------------------

	// init
	void InitCatmullRom();
	void SaveCatmullRom();

	// -------- 前半攻撃処理 --------//
	void FirstHalhUpdateMoveBack(BasePlayerParts* parts);
	void FirstHalhUpdateRotationBack(BasePlayerParts* parts);

	void FirstHalhUpdateMoveFront(BasePlayerParts* parts);
	void FirstHalhUpdateRotationFront(BasePlayerParts* parts);

	void FirstHalhWaitMoveTime();

	// -------- 後半攻撃処理 --------//
	void SecondHalfUpdateMoveCatmullRom(BasePlayerParts* parts);
};