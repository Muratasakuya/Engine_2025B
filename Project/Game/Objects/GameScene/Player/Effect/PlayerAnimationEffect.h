#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Game/GameEffect.h>

// front
class Player;

//============================================================================
//	PlayerAnimationEffect class
//============================================================================
class PlayerAnimationEffect {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAnimationEffect() = default;
	~PlayerAnimationEffect() = default;

	void Init(const Player& player);

	void Update(Player& player);

	// editor
	void ImGui(const Player& player);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- stricture ----------------------------------------------------

	// アニメーションの名前
	enum class AnimationKey {

		None,
		Attack_1st,
		Attack_2nd,
		Attack_3rd
	};

	// 斬撃
	struct Slash {

		float scaling;
		Vector3 translation;
		Vector3 rotation;
	};

	//--------- variables ----------------------------------------------------

	// 斬撃
	std::unique_ptr<GameEffect> slashEffect;

	// 1段目の攻撃
	Slash firstSlashParam_;
	// 2段目の攻撃
	static const uint32_t secondSlashCount_ = 2;
	std::array<Slash, secondSlashCount_> secondSlashParams_;
	// 3段階目の攻撃
	// 保留、アクションを完成しきってからエフェクトを追加する
	Slash thirdSlashParam_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateAnimationKey(Player& player);
	void UpdateEmit(Player& player);
};