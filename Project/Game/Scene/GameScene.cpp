#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Load() {

	// cubeMap、.dds
	asset_->LoadTexture("overcast_soil_puresky_4k");

	// lut
	asset_->LoadLutTexture("lut_cool");

	// player
	asset_->LoadModel("playerBody");
	asset_->LoadModel("playerLeftHand");
	asset_->LoadModel("playerRightHand");
	asset_->LoadModel("playerSword");

	// environment
	asset_->LoadModel("field");
	asset_->LoadModel("fence");
	asset_->LoadModel("loftNurbs");
	asset_->LoadModel("rustyMetal");
	asset_->LoadModel("wall");
}

void GameScene::Init() {

	//========================================================================
	//	load
	//========================================================================

	Load();

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessSystem_->Create({ 
		PostProcessType::Bloom,PostProcessType::Lut });
	postProcessSystem_->AddProcess(PostProcessType::Bloom);
	postProcessSystem_->AddProcess(PostProcessType::Lut);

	postProcessSystem_->InputProcessTexture("lut_cool", PostProcessType::Lut, asset_);

	//========================================================================
	//	sceneObject
	//========================================================================

	// camera
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	sceneView_->SetCamera(followCamera_.get());

	// light
	gameLight_ = std::make_unique<PunctualLight>();
	gameLight_->Init();
	gameLight_->directional.direction.x = -0.306f;
	gameLight_->directional.direction.y = -0.836f;
	gameLight_->directional.direction.z = -0.455f;
	gameLight_->directional.color = Color::Convert(0xaeeefdff);

	sceneView_->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	ECSManager::GetInstance()->CreateSkybox("overcast_soil_puresky_4k");

	player_ = std::make_unique<Player>();
	player_->Init(followCamera_.get());

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());
	followCamera_->FirstUpdate();

	// editor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset_);

	levelEditor_ = std::make_unique<LevelEditor>();
	levelEditor_->Init("levelEditor");
}

void GameScene::Update() {

	followCamera_->Update();

	player_->Update();

	// editor
	entityEditor_->Update();
	levelEditor_->Update();
}

void GameScene::ImGui() {
}