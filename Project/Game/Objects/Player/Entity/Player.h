#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity3D.h>

// weapon
#include <Game/Objects/Player/Entity/PlayerWeapon.h>
// state
#include <Game/Objects/Player/State/PlayerStateController.h>
// collision
#include <Game/Objects/Player/Collision/PlayerAttackCollision.h>
// HUD
#include <Game/Objects/Player/HUD/PlayerHUD.h>
#include <Game/Objects/Player/HUD/PlayerStunHUD.h>

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
	void OnCollisionEnter(const CollisionBody* collisionBody) override;

	//--------- accessor -----------------------------------------------------

	void SetBossEnemy(const BossEnemy* bossEnemy);
	void SetFollowCamera(FollowCamera* followCamera);
	void SetPostProcessSystem(PostProcessSystem* postProcessSystem);

	PlayerState GetCurrentState() const { return stateController_->GetCurrentState(); }

	PlayerAttackCollision* GetAttackCollision() const { return playerAttackCollision_.get(); }
	GameEntity3D* GetAlly() const { return ally_.get(); }
	PlayerHUD* GetHUD() const { return hudSprites_.get(); }
	PlayerStunHUD* GetStunHUD() const { return stunHudSprites_.get(); }

	int GetDamage() const;
	int GetToughness() const { return stats_.toughness; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const BossEnemy* bossEnemy_;

	// 使用する武器
	std::unique_ptr<PlayerWeapon> rightWeapon_; // 右手
	std::unique_ptr<PlayerWeapon> leftWeapon_;  // 左手
	// 味方
	std::unique_ptr<GameEntity3D> ally_;

	// 状態の管理
	std::unique_ptr<PlayerStateController> stateController_;

	// 攻撃の衝突
	std::unique_ptr<PlayerAttackCollision> playerAttackCollision_;

	// HUD
	std::unique_ptr<PlayerHUD> hudSprites_;
	std::unique_ptr<PlayerStunHUD> stunHudSprites_;

	// parameters
	Transform3DComponent initTransform_; // 初期化時の値
	PlayerStats stats_; // ステータス

	// 敵のスタン中の更新になったか
	bool isStunUpdate_;

	// editor
	int editingStateIndex_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitWeapon();
	void InitAnimations();
	void InitCollision();
	void InitState();
	void InitHUD();

	// helper
	void SetInitTransform();
	void CheckBossEnemyStun();
};