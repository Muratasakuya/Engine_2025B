#include "CameraManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	CameraManager classMethods
//============================================================================

void CameraManager::Init() {

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Init();
#endif // _DEBUG


	// 影生成用
	lightViewCamera_ = std::make_unique<LightViewCamera>();
	lightViewCamera_->Init();
}

void CameraManager::Update() {

#ifdef _DEBUG
	debugCamera_->Update();
#endif // _DEBUG

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