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
	int GetDamage() const;
	int GetToughness() const { return stats_.toughness; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 使用する武器
	std::unique_ptr<PlayerWeapon> rightWeapon_; // 右手
	std::unique_ptr<PlayerWeapon> leftWeapon_;  // 左手

	// 状態の管理
	std::unique_ptr<PlayerStateController> stateController_;

	// 攻撃の衝突
	std::unique_ptr<PlayerAttackCollision> playerAttackCollision_;

	// HUD
	std::unique_ptr<PlayerHUD> hudSprites_;

	// parameters
	Transform3DComponent initTransform_; // 初期化時の値
	PlayerStats stats_; // ステータス

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
};