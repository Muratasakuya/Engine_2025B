#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Particle/ParticleSystem.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Load(Asset* asset) {

	// particleTexture
	asset->LoadTexture("circle");
	asset->LoadTexture("redCircle");
	asset->LoadTexture("white");
	asset->LoadTexture("monsterBall");
	asset->LoadTexture("noise");
	asset->LoadTexture("smallCircle");
	asset->LoadTexture("spark");
	asset->LoadTexture("gradationLine_1");
	asset->LoadTexture("gradationLine_0");
	asset->LoadTexture("effectCircle");
	asset->LoadTexture("uvChecker");
	// cubeMap、.dds
	asset->LoadTexture("docklands_01_2k");

	// particleModel
	asset->LoadModel("cube");
	asset->LoadModel("axis");
	asset->LoadModel("billboardPlane");

	// player
	asset->LoadModel("playerBody");
	asset->LoadModel("playerLeftHand");
	asset->LoadModel("playerRightHand");
	asset->LoadModel("playerSword");

	// environment
	asset->LoadModel("stageField");

	// animation
	asset->LoadModel("BrainStem");
	asset->LoadAnimation("BrainStem", "BrainStem");
}

void GameScene::Init(
	[[maybe_unused]] Asset* asset,
	[[maybe_unused]] CameraManager* cameraManager,
	[[maybe_unused]] LightManager* lightManager,
	[[maybe_unused]] PostProcessSystem* postProcessSystem
) {

	//========================================================================
	//	load
	//========================================================================

	Load(asset);

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessSystem_ = postProcessSystem;
	postProcessSystem_->Create({ PostProcessType::Bloom });

	postProcessSystem_->AddProcess(PostProcessType::Bloom);

	//========================================================================
	//	sceneObject
	//========================================================================

	// camera
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Init();

	cameraManager->SetCamera(followCamera_.get());

	// light
	gameLight_ = std::make_unique<PunctualLight>();
	gameLight_->Init();

	lightManager->SetLight(gameLight_.get());

	//========================================================================
	//	initObject
	//========================================================================

	player_ = std::make_unique<Player>();
	player_->Init(followCamera_.get());

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());

	// skybox
	Skybox::GetInstance()->Create(asset->GetTextureGPUIndex("docklands_01_2k"));

	// entityEditor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset);
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();

	// entityEditor
	entityEditor_->Update();
}

void GameScene::ImGui() {
}