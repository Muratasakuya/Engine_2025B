#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity3D.h>

// state
#include <Game/Objects/Player/State/PlayerStateController.h>
// HUD
#include <Game/Objects/Player/HUD/PlayerHUD.h>

//============================================================================
//	Player class
//============================================================================
class Player :
	public GameEntity3D {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Player() = default;
	~Player() = default;

	void DerivedInit() override;

	void Update() override;

	void DerivedImGui() override;

	/*-------- collision ----------*/

	// 衝突コールバック関数
	void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) override;

	//--------- accessor -----------------------------------------------------

	void SetBossEnemy(const BossEnemy* bossEnemy);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const BossEnemy* bossEnemy_;

	// 状態の管理
	std::unique_ptr<PlayerStateController> stateController_;

	// HUD
	std::unique_ptr<PlayerHUD> hudSprites_;

	// parameters
	Transform3DComponent initTransform_; // 初期化時の値
	PlayerStats stats_; // ステータス

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitAnimations();
	void InitCollision();
	void InitState();
	void InitHUD();

	// helper
	void SetInitTransform();
};