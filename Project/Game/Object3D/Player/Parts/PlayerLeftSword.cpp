#include "PlayerLeftSword.h"

//============================================================================
//	PlayerLeftSword classMethods
//============================================================================

void PlayerLeftSword::Init() {

	BasePlayerParts::Init("playerSword", "playerLeftSword");

	// json適応
	ApplyJson();
}

void PlayerLeftSword::ImGui() {

	parameter_.ImGui();
}

void PlayerLeftSword::ApplyJson() {
}

void PlayerLeftSword::SaveJson() {

	parameter_.SaveJson();
}