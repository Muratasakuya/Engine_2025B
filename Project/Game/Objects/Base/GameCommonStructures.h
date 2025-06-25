#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector2.h>

// c++
#include <string>

//============================================================================
//	GameCommonStructures class
//============================================================================

namespace GameCommon {

	// HUDの初期化値
	struct HUDInitParameter {

		Vector2 translation; // 座標

		// imgui
		bool ImGui(const std::string& label);

		// json
		void ApplyJson(const Json& data);
		void SaveJson(Json& data);
	};
}