#include "TitleScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Scene/SceneView.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void TitleScene::Init() {

	//========================================================================
	//	postProcess
	//========================================================================

	// 初期化時にのみ作成できる
	PostProcessSystem::GetInstance()->Create({
		PostProcessType::RadialBlur,
		PostProcessType::Bloom,
		PostProcessType::CRTDisplay});

	//========================================================================
	//	scene
	//========================================================================

	// カメラの設定
	camera3D_ = std::make_unique<BaseCamera>();
	sceneView_->SetGameCamera(camera3D_.get());

	// ライトの設定
	light_ = std::make_unique<PunctualLight>();
	light_->Init();
	sceneView_->SetLight(light_.get());
}

void TitleScene::Update() {

	camera3D_->UpdateView();

	LineRenderer::GetInstance()->DrawGrid(32, 32.0f, Color::White());
}