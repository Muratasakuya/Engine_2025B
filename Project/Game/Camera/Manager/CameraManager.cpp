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

	// 追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();
	// ゲーム開始時のカメラ
	beginGameCamera_ = std::make_unique<BeginGameCamera>();
	beginGameCamera_->Init();
	// ゲーム終了時のカメラ
	endGameCamera_ = std::make_unique<EndGameCamera>();
	endGameCamera_->Init();

	// 最初のカメラを設定する
	sceneView_->SetGameCamera(followCamera_.get());

#if defined(_DEBUG) || defined(_DEVELOPBUILD)

	// シーン視点のカメラに追加する
	sceneView_->AddSceneCamera("DefaultFollowCamera", followCamera_.get());
	sceneView_->AddSceneCamera("BeginGameCamera", beginGameCamera_.get());
	sceneView_->AddSceneCamera("EndGameCamera", endGameCamera_.get());
#endif
}

void CameraManager::SetTarget(const Player* Player) {

	player_ = nullptr;
	player_ = Player;

	// 追従先を設定する: player
	followCamera_->SetTarget(FollowCameraTargetType::Player, player_->GetTransform());
	followCamera_->SetTarget(FollowCameraTargetType::PlayerAlly, player_->GetAlly()->GetTransform());
}

void CameraManager::Update(GameSceneState sceneState) {

	// シーンの状態に応じた更新処理
	switch (sceneState) {
	case GameSceneState::Start:

		followCamera_->Update();
		break;
	case GameSceneState::BeginGame:

		beginGameCamera_->Update();
		break;
	case GameSceneState::PlayGame:

		followCamera_->Update();
		break;
	case GameSceneState::EndGame:

		endGameCamera_->Update();
		break;
	}
	// シーン状態のチェック
	CheckSceneState(sceneState);
}

void CameraManager::CheckSceneState(GameSceneState sceneState) {

	// シーンが切り替わったとき
	if (preSceneState_ != sceneState) {
		switch (preSceneState_) {
		case GameSceneState::Start:

			// カメラを変更する
			sceneView_->SetGameCamera(beginGameCamera_.get());
			break;
		case GameSceneState::BeginGame:

			// カメラを変更する
			sceneView_->SetGameCamera(followCamera_.get());
			break;
		case GameSceneState::PlayGame:

			// カメラを変更する
			sceneView_->SetGameCamera(endGameCamera_.get());
			break;
		case GameSceneState::EndGame:
			break;
		}
	}
	preSceneState_ = sceneState;
}

void CameraManager::ImGui() {
}