#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>

// entities
#include <Game/Object3D/Enemy/Boss/Entity/BossEnemy.h>
#include <Game/Object3D/Enemy/Boss/Entity/BossEnemyWeapon.h>

//============================================================================
//	BossEnemyManager class
//============================================================================
class BossEnemyManager :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyManager() :IGameEditor("BossEnemyManager") {};
	~BossEnemyManager() = default;

	void Init();

	void Update();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// entities
	std::unique_ptr<BossEnemy> bossEnemy_;             // ボスの実体
	std::unique_ptr<BossEnemyWeapon> bossEnemyWeapon_; // 武器

	//--------- functions ----------------------------------------------------

	// init
	void InitEntities();

	// upodate
	void UpdateEntities();
};