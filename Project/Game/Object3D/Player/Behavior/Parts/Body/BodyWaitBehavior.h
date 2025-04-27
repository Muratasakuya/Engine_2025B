#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/Parts/Base/IPlayerBehavior.h>

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

	// parameter
	// ふわふわさせるための変数
	float elapsed_;   // 時間経過
	float amplitude_; // 振幅
	float speed_;     // 速さ
};