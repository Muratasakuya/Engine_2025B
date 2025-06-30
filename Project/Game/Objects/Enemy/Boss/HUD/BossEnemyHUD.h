#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameHPBar.h>
#include <Game/Objects/Base/GameDigitDisplay.h>
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
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// ダメージ表示の構造体
	struct DamagePopup {

		std::unique_ptr<GameDigitDisplay> digits;
		Vector2 basePos; // 表示座標
		float timer;     // 表示時間経過
		float outTimer;  // 消えるときの時間経過
		bool active;     // 表示している間
		int value;       // ダメージ値
	};

	//--------- variables ----------------------------------------------------

	const FollowCamera* followCamera_;

	// ステータス
	BossEnemyStats stats_;
	// 受けたダメージ
	std::deque<int> receivedDamages_;

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

	// ダメージの表示
	std::vector<DamagePopup> damagePopups_;
	// 最大ダメージ表示数
	const uint32_t damageDisplayMaxNum_ = 8;
	// 最大桁数
	const uint32_t damageDigitMaxNum_ = 4;

	float totalAppearDuration_;
	float damageStayTime_;                // 全ての桁を表示しきった後の待機時間
	float damageDisplayTime_;             // ダメージ表示の時間
	float damageOutTime_;                 // ダメージ表示の消える時間
	float digitDisplayInterval_;          // 桁表示の時間差
	float damageNumSpacing_;              // 数字の間の幅
	float bossScreenPosOffsetY_;          // スクリーン座標のYオフセット
	float maxDamageEmissive_;             // 最大発光度
	Vector2 damageDisplayPosRandomRange_; // ダメージ表示のランダム範囲
	Vector2 damageDisplayMaxSize_;        // ダメージ表示の最大サイズ
	Vector2 damageDisplaySize_;           // ダメージ表示のサイズ
	EasingType damageDisplayEasingType_;  // ダメージ表示のイージングタイプ
	EasingType damageOutEasingType_;      // 消えるときのイージングタイプ

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitSprite();

	// update
	void UpdateSprite(const BossEnemy& bossEnemy);
	void UpdateDamagePopups(const BossEnemy& bossEnemy);
};