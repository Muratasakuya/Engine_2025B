#include "ClearScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Scene/Manager/SceneManager.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Input/Input.h>

//============================================================================
//	ClearScene classMethods
//============================================================================

void ClearScene::Init() {

	//========================================================================
	//	scene
	//========================================================================

	// カメラの設定
	camera3D_ = std::make_unique<BaseCamera>();
	camera3D_->UpdateView();
	sceneView_->SetGameCamera(camera3D_.get());

	// ライトの設定
	light_ = std::make_unique<PunctualLight>();
	light_->Init();
	sceneView_->SetLight(light_.get());

	// 遷移の設定
	fadeTransition_ = std::make_unique<FadeTransition>();
	fadeTransition_->Init();
}

void ClearScene::Update() {

	//========================================================================
	//	sceneEvent
	//========================================================================

	// 仮でA、スペースキーでタイトル
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerGamepadButton(GamePadButtons::A) &&
		fadeTransition_ && sceneManager_->IsFinishedTransition()) {

		sceneManager_->SetNextScene(Scene::Title, std::move(fadeTransition_));
	}

	//========================================================================
	//	debug
	//========================================================================

	LineRenderer::GetInstance()->DrawGrid(32, 32.0f, Color::White());
}