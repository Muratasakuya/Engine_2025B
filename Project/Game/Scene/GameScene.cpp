#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	GameScene classMethods
//============================================================================

void GameScene::Init() {

	//========================================================================
	//	postProcess
	//========================================================================

	PostProcessSystem* postProcessSystem = PostProcessSystem::GetInstance();

	postProcessSystem->Create({
			PostProcessType::RadialBlur,
			PostProcessType::Bloom });
	postProcessSystem->AddProcess(PostProcessType::RadialBlur);
	postProcessSystem->AddProcess(PostProcessType::Bloom);

	// ブラーの値を0.0fで初期化
	RadialBlurForGPU radialBlurParam{};
	radialBlurParam.center = Vector2(0.5f, 0.5f);
	radialBlurParam.numSamples = 0;
	radialBlurParam.width = 0.0f;
	postProcessSystem->SetParameter(radialBlurParam, PostProcessType::RadialBlur);

	//========================================================================
	//	sceneObject
	//========================================================================

	// camera
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Init(sceneView_);

	// light
	gameLight_ = std::make_unique<PunctualLight>();
	gameLight_->Init();
	gameLight_->directional.direction.x = 0.558f;
	gameLight_->directional.direction.y = -0.476f;
	gameLight_->directional.direction.z = -0.68f;
	gameLight_->directional.color = Color::Convert(0xadceffff);

	sceneView_->SetLight(gameLight_.get());

	//========================================================================
	//	backObjects
	//========================================================================

	ObjectManager::GetInstance()->CreateSkybox("overcast_soil_puresky_4k");

	//========================================================================
	//	editor
	//========================================================================

	levelEditor_ = std::make_unique<LevelEditor>();
	levelEditor_->Init("levelEditor");

	//========================================================================
	//	frontObjects
	//========================================================================

	player_ = std::make_unique<Player>();
	player_->Init("player", "player", "Player", "player_idle");

	// 追従先を設定する: player
	cameraManager_->SetTarget(player_.get());

	bossEnemy_ = std::make_unique<BossEnemy>();
	bossEnemy_->Init("bossEnemy", "bossEnemy", "Enemy", "bossEnemy_idle");

	// player、カメラをセット
	bossEnemy_->SetPlayer(player_.get());
	bossEnemy_->SetFollowCamera(cameraManager_->GetFollowCamera());
	// bossEnemy、カメラをセット
	player_->SetBossEnemy(bossEnemy_.get());
	player_->SetFollowCamera(cameraManager_->GetFollowCamera());
}

void GameScene::Update() {

	//========================================================================
	//	object
	//========================================================================

	bossEnemy_->Update();
	player_->Update();

	//========================================================================
	//	sceneObject
	//========================================================================

	cameraManager_->Update();

	//========================================================================
	//	editor
	//========================================================================

	// editor
	levelEditor_->Update();
}

void GameScene::ImGui() {
}