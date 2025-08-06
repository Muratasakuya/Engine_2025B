#include "CameraManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Scene/SceneView.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>

//============================================================================
//	CameraManager classMethods
//============================================================================

void CameraManager::Init(SceneView* sceneView) {

	sceneView_ = nullptr;
	sceneView_ = sceneView;

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	// 最初のカメラを設定する
	sceneView_->SetGameCamera(followCamera_.get());

#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	// シーン視点のカメラに追加する
	sceneView_->AddSceneCamera("DefaultFollowCamera", followCamera_.get());
#endif
}

void CameraManager::SetTarget(const Player* Player) {

	player_ = nullptr;
	player_ = Player;

	// 追従先を設定する: player
	followCamera_->SetTarget(FollowCameraTargetType::Player, player_->GetTransform());
	followCamera_->SetTarget(FollowCameraTargetType::PlayerAlly, player_->GetAlly()->GetTransform());
}

void CameraManager::Update() {

	followCamera_->Update();
}

void CameraManager::ImGui() {


}