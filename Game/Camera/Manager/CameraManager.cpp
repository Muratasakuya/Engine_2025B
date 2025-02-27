#include "CameraManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	CameraManager classMethods
//============================================================================

void CameraManager::Init() {

	// 影生成用
	lightViewCamera_ = std::make_unique<LightViewCamera>();
	lightViewCamera_->Init();
}

void CameraManager::Update() {

	lightViewCamera_->Update();
}

void CameraManager::SetCamera(BaseCamera* gameCamera) {

	// カメラのセット
	gameCamera_ = std::nullopt;
	gameCamera_ = gameCamera;
}

BaseCamera* CameraManager::GetCamera() const {

	if (!gameCamera_.has_value()) {
		ASSERT(FALSE, "un setting gameCamera");
		return nullptr;
	}
	return *gameCamera_;
}