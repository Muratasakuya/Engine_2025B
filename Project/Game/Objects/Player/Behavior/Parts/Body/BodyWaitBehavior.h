#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Behavior/Parts/Base/IPlayerBehavior.h>

//============================================================================
//	BodyWaitBehavior class
//============================================================================
class BodyWaitBehavior :
	public IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BodyWaitBehavior(const Json& data);
	~BodyWaitBehavior() = default;

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

	std::optional<Vector3> baseTranslate_; // 初期座標

	// Y座標を元に戻す
	std::unique_ptr<SimpleAnimation<float>> undoPosY_;

	// parameter
	// ふわふわさせるための変数
	float basePosY_;  // 規定のY座標
	float elapsed_;   // 時間経過
	float amplitude_; // 振幅
	float speed_;     // 速さ
	// 回転補間割合
	float rotationLerpRate_;
};