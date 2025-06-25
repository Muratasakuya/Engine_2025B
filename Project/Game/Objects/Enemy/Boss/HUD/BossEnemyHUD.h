#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameHPBar.h>
#include <Game/Objects/Base/GameDigitDisplay.h>
#include <Game/Objects/Base/GameCommonStructures.h>
#include <Game/Objects/Enemy/Boss/Structures/BossEnemyStructures.h>

//============================================================================
//	BossEnemyHUD class
//============================================================================
class BossEnemyHUD {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyHUD() = default;
	~BossEnemyHUD() = default;

	void Init();

	void Update();

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetStatas(const BossEnemyStats& stats) { stats_ = stats; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// ステータス
	BossEnemyStats stats_;

	// HP背景
	std::unique_ptr<GameEntity2D> hpBackground_;
	GameCommon::HUDInitParameter hpBackgroundParameter_;
	// HP残量
	std::unique_ptr<GameHPBar> hpBar_;
	GameCommon::HUDInitParameter hpBarParameter_;
	// 撃破靭性値
	std::unique_ptr<GameHPBar> destroyBar_;
	GameCommon::HUDInitParameter destroyBarParameter_;
	// 撃破靭性値の数字表示
	std::unique_ptr<GameDigitDisplay> destroyNumDisplay_;
	GameCommon::HUDInitParameter destroyNumParameter_;
	Vector2 destroyNumOffset_; // オフセット座標
	Vector2 destroyNumSize_;   // サイズ
	// 名前文字表示
	std::unique_ptr<GameEntity2D> nameText_;
	GameCommon::HUDInitParameter nameTextParameter_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitSprite();

	// update
	void UpdateSprite();
};