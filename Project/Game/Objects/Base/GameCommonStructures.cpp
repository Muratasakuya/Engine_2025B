#include "GameCommonStructures.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	GameCommonStructures classMethods
//============================================================================

bool GameCommon::HUDInitParameter::ImGui(const std::string& label) {

	bool edit = false;

	ImGui::SeparatorText(label.c_str());

	edit = ImGui::DragFloat2(("translation##" + label).c_str(), &translation.x, 1.0f);

	return edit;
}

void GameCommon::HUDInitParameter::ApplyJson(const Json& data) {

	translation = translation.FromJson(data["translation"]);
}

void GameCommon::HUDInitParameter::SaveJson(Json& data) {

	data["translation"] = translation.ToJson();
}