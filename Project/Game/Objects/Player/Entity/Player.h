#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity3D.h>

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

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// HUD
	std::unique_ptr<PlayerHUD> hudSprites_;

	// parameters
	PlayerStats stats_; // ステータス

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	void InitHUD();
};