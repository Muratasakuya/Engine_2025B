#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Particle/ParticleSystem.h>
#include <Engine/Scene/SceneView.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Load() {

	// cubeMap、.dds
	asset_->LoadTexture("docklands_01_2k");
	// lut
	asset_->LoadLutTexture("lut_hot");

	// player
	asset_->LoadModel("playerBody");
	asset_->LoadModel("playerLeftHand");
	asset_->LoadModel("playerRightHand");
	asset_->LoadModel("playerSword");

	// environment
	asset_->LoadModel("stageField");

	// animation
	asset_->LoadModel("BrainStem");
	asset_->LoadAnimation("BrainStem", "BrainStem");
}

void GameScene::Init() {

	//========================================================================
	//	load
	//========================================================================

	Load();

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessSystem_->Create({ PostProcessType::Bloom });
	postProcessSystem_->AddProcess(PostProcessType::Bloom);

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

	sceneView_->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	player_ = std::make_unique<Player>();
	player_->Init(followCamera_.get());

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());

	// entityEditor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset_);
	// levelEditor
	levelEditor_ = std::make_unique<LevelEditor>();
	levelEditor_->LoadFile(asset_);

	// 仮の地面
	uint32_t id = ECSManager::GetInstance()->CreateObject3D("stageField", "field", "Environment");
	auto transform = ECSManager::GetInstance()->GetComponent<Transform3DComponent>(id);
	transform->translation.y = -0.8f;
	transform->scale.x = 128.0f;
	transform->scale.z = 128.0f;
	auto material = ECSManager::GetInstance()->GetComponent<MaterialComponent, true>(id);
	material->front().uvMatrix = Matrix4x4::MakeAffineMatrix(Vector3(24.0f, 24.0f, 0.0f),
		Vector3::AnyInit(0.0f), Vector3::AnyInit(0.0f));
}

void GameScene::Update() {

	followCamera_->Update();

	player_->Update();

	// entityEditor
	entityEditor_->Update();
}

void GameScene::ImGui() {
}