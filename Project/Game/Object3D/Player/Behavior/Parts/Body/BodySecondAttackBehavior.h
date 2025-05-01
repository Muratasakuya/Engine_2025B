#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodySecondAttackBehavior class
//============================================================================
class BodySecondAttackBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodySecondAttackBehavior(const Json& data);
	~BodySecondAttackBehavior() = default;

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

	//--------- structure ----------------------------------------------------

	// 移動に使う変数
	struct MoveStructure {

		// 進む量
		float moveValue;
		// 進行方向
		Vector3 direction;
		// 進行方向に向かって直進させる
		std::unique_ptr<SimpleAnimation<Vector3>> moveAnimation;
	};

	// 現在の攻撃状態
	enum class State {

		FrontLeftStep,
		FrontRightStep,
		ReturnCenterStep,
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State currentState_;

	// 最初の前方ベクトル
	Vector3 forwardDirection_;

	// parameter
	// 左前に行くとき
	MoveStructure moveFrontLeft_;
	// 左前行った後に右前に行くとき、ここが一番移動量多め
	MoveStructure moveFrontRight_;
	// 右前行った後に左前に行って正面に戻る
	MoveStructure moveCenter_;

	//--------- functions ----------------------------------------------------

	void UpdateFrontLeftStep(BasePlayerParts* parts);
	void UpdateFrontRightStep(BasePlayerParts* parts);
	void UpdateReturnCenterStep(BasePlayerParts* parts);

	void Start(BasePlayerParts* parts, MoveStructure& moveStructure);

	const char* ToStringState(State state);
};