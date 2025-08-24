#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/GameEffect.h>

// front
class BossEnemy;

//============================================================================
//	BossEnemyAnimationEffect class
//============================================================================
class BossEnemyAnimationEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BossEnemyAnimationEffect() = default;
	~BossEnemyAnimationEffect() = default;

	void Init(const BossEnemy& bossEnemy);

	void Update(BossEnemy& bossEnemy);

	// editor
	void ImGui(const BossEnemy& bossEnemy);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- stricture ----------------------------------------------------

	// アニメーションの名前
	enum class AnimationKey {

		None,
		LightAttack,
		StrongAttack,
	};

	// 斬撃
	struct Slash {

		Vector3 translation;
		Vector3 rotation;
		std::unique_ptr<GameEffect> effect;
	};

	//--------- variables ----------------------------------------------------

	// 現在のアニメーション
	AnimationKey currentAnimationKey_;
	AnimationKey editAnimationKey_;

	// 弱斬撃
	Slash lightSlash_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateAnimationKey(BossEnemy& bossEnemy);
	void UpdateEmit(BossEnemy& bossEnemy);
};