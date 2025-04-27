#include "PlayerRightSword.h"

//============================================================================
//	PlayerRightSword classMethods
//============================================================================

void PlayerRightSword::Init() {

	BasePlayerParts::Init("playerSword", "playerRightSword");

	// json適応
	ApplyJson();
}

void PlayerRightSword::ImGui() {

	parameter_.ImGui();
}

void PlayerRightSword::ApplyJson() {
}

void PlayerRightSword::SaveJson() {

	parameter_.SaveJson();
}