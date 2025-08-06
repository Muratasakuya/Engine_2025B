#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/GameScene/Player/State/Interface/PlayerBaseAttackState.h>
#include <Engine/Effect/GameEffect.h>

//============================================================================
//	PlayerAttack_1stState class
//============================================================================
class PlayerAttack_1stState :
	public PlayerBaseAttackState {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerAttack_1stState();
	~PlayerAttack_1stState() = default;

	void Enter(Player& player) override;

	void Update(Player& player) override;

	void Exit(Player& player) override;

	// imgui
	void ImGui(const Player& player) override;

	// json
	void ApplyJson(const Json& data) override;
	void SaveJson(Json& data) override;

	//--------- accessor -----------------------------------------------------

	bool GetCanExit() const override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// parameters
	// 斬撃エフェクト
	Vector3 slashEffectRotation_;   // 回転
	Vector3 slashEffectTranslaton_; // 座標

	// 斬撃
	std::unique_ptr<GameEffect> slashEffect_;
};