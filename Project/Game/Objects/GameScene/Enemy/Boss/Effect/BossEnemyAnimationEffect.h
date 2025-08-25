#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/GameEffect.h>

// front
class BossEnemy;
class FollowCamera;

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

	//--------- accessor -----------------------------------------------------

	void SetFollowCamera(const FollowCamera* followCamera);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- stricture ----------------------------------------------------

	// アニメーションの名前
	enum class AnimationKey {

		None,
		Move,
		LightAttack,
		StrongAttack,
		ChargeAttack,
	};

	// 斬撃
	struct Slash {

		Vector3 translation;
		Vector3 rotation;
		std::unique_ptr<GameEffect> effect;
	};

	// 発生
	struct Emit {

		bool emitEnble = true;
		Vector3 translation;
		std::unique_ptr<GameEffect> effect;
	};

	//--------- variables ----------------------------------------------------

	const FollowCamera* followCamera_;

	// 現在のアニメーション
	AnimationKey currentAnimationKey_;
	AnimationKey editAnimationKey_;

	// 弱斬撃
	Slash lightSlash_;
	// 強斬撃
	Slash strongSlash_;

	// チャージ
	Emit chargeStar_;   // 星
	Emit chargeCircle_; // 集まってくるエフェクト

	// 移動時の巻き風
	Emit moveWind_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateAnimationKey(BossEnemy& bossEnemy);
	void UpdateEmit(BossEnemy& bossEnemy);
	void UpdateAllways();

	// helper
	void EmitChargeEffect(const BossEnemy& bossEnemy);
};