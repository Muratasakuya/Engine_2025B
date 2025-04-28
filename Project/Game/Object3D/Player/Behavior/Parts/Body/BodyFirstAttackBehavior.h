#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

// 動かし方
// 左後ろに回転(下にかがみながら) -> 正面に向かせる(右重心にできるといい)

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

	// parameter
	// 進む量
	float moveValue_;
	// 向いている方向に向かって直進させる
	std::unique_ptr<SimpleAnimation<Vector3>> moveStraight_;
	// 回転角
	Vector3 rotationAngle_;
	// 左後ろに回転(下にかがみながら)
	std::unique_ptr<SimpleAnimation<Vector3>> backLeftRotation_;
	std::optional<Quaternion> startRotation_;
	// 正面に向かせる(右重心にできるといい)
	std::unique_ptr<SimpleAnimation<Vector3>> forwardRightShift_;

	//--------- functions ----------------------------------------------------

	void UpdateMoveStraight(BasePlayerParts* parts);
	void UpdateBackLeftRotation(BasePlayerParts* parts);
	void UpdateForwardRightShift(BasePlayerParts* parts);
};