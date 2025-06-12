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
	asset_->LoadTexture("kloppenheim_02_puresky_2k");

	// lut
	asset_->LoadLutTexture("lut_cool");

	// player
	asset_->LoadModel("playerBody");
	asset_->LoadModel("playerLeftHand");
	asset_->LoadModel("playerRightHand");
	asset_->LoadModel("playerSword");

	// environment
	asset_->LoadModel("stageField");
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
	gameLight_->directional.direction.x = 1.0f;
	gameLight_->directional.direction.z = 1.0f;
	gameLight_->directional.direction.y = -0.5f;

	sceneView_->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	player_ = std::make_unique<Player>();
	player_->Init(followCamera_.get());

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());
	followCamera_->FirstUpdate();

	// entityEditor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset_);

	//// 仮の地面
	//uint32_t id = ECSManager::GetInstance()->CreateObject3D("stageField", "field", "Environment");
	//auto transform = ECSManager::GetInstance()->GetComponent<Transform3DComponent>(id);
	//transform->translation.y = -0.8f;
	//transform->scale.x = 128.0f;
	//transform->scale.z = 128.0f;
	//auto material = ECSManager::GetInstance()->GetComponent<MaterialComponent, true>(id);
	//material->front().uvMatrix = Matrix4x4::MakeAffineMatrix(Vector3(24.0f, 24.0f, 0.0f),
	//	Vector3::AnyInit(0.0f), Vector3::AnyInit(0.0f));

	//ECSManager::GetInstance()->CreateSkybox("kloppenheim_02_puresky_2k");
}

void GameScene::Update() {

	followCamera_->Update();

	player_->Update();

	// entityEditor
	entityEditor_->Update();
}

void GameScene::ImGui() {
}