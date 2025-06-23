#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameHPBar.h>
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

	//--------- structure ----------------------------------------------------

	// 各spriteごとの初期化値
	struct InitParameter {

		Vector2 translation; // 座標

		// imgui
		bool ImGui(const std::string& label);

		// json
		void ApplyJson(const Json& data);
		void SaveJson(Json& data);
	};

	//--------- variables ----------------------------------------------------

	// 現在のHP
	BossEnemyStats stats_;

	// HP背景
	std::unique_ptr<GameEntity2D> hpBackground_;
	InitParameter hpBackgroundParameter_;
	// HP残量
	std::unique_ptr<GameHPBar> hpBar_;
	InitParameter hpBarParameter_;
	// 名前文字表示
	std::unique_ptr<GameEntity2D> nameText_;
	InitParameter nameTextParameter_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitSprite();

	// update
	void UpdateSprite();

	// helper
	void SetInitParameter(GameEntity2D& sprite, InitParameter parameter);
};