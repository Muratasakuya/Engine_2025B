#include "LightManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	LightManager classMethods
//============================================================================

void LightManager::ImGui() {

	if (gameLight_.has_value()) {

		// 現在使われているライトの表示
		gameLight_.value()->ImGui();
	}
}

void LightManager::SetLight(PunctualLight* gameLight) {

	// ライトのセット
	gameLight_ = std::nullopt;
	gameLight_ = gameLight;
}

PunctualLight* LightManager::GetLight() const {

	if (!gameLight_.has_value()) {
		ASSERT(FALSE, "un setting gameLight");
		return nullptr;
	}
	return *gameLight_;
}