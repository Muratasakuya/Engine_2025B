#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameHPBar.h>
#include <Game/Objects/Base/GameDisplayDamage.h>
#include <Game/Objects/Base/GameCommonStructures.h>
#include <Game/Objects/Enemy/Boss/Structures/BossEnemyStructures.h>

// c++
#include <deque>
// front
class BossEnemy;
class FollowCamera;

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

	void Update(const BossEnemy& bossEnemy);

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetStatas(const BossEnemyStats& stats) { stats_ = stats; }
	void SetDamage(int damage);
	void SetFollowCamera(const FollowCamera* followCamera) { followCamera_ = followCamera; }
	void SetDisable();
	void SetValid();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const FollowCamera* followCamera_;

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
	// ダメージ表示
	std::unique_ptr<GameDisplayDamage> damageDisplay_;

	// parameters
	float returnAlphaTimer_; // alpha値を元に戻すときの経過時間
	float returnAlphaTime_;  // alpha値を元に戻すときの時間
	EasingType returnAlphaEasingType_;

	bool isDisable_;   // 無効状態かどうか
	bool returnVaild_; // 再度有効にする

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitSprite();

	// update
	void UpdateSprite(const BossEnemy& bossEnemy);
	void UpdateAlpha();
};