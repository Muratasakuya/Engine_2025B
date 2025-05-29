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
	// cubeMap、.dds
	asset->LoadTexture("docklands_01_2k");

	// particleModel
	asset->LoadModel("cube");
	asset->LoadModel("axis");
	asset->LoadModel("billboardPlane");
	asset->LoadModel("cylinder");

	// player
	asset->LoadModel("playerBody");
	asset->LoadModel("playerLeftHand");
	asset->LoadModel("playerRightHand");
	asset->LoadModel("playerSword");

	// environment
	asset->LoadModel("stageField");
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

	// 仮の地面
	uint32_t id = ECSManager::GetInstance()->CreateObject3D("stageField", "field", "Environment");
	auto transform = ECSManager::GetInstance()->GetComponent<Transform3DComponent>(id);
	transform->scale.x = 128.0f;
	transform->scale.z = 128.0f;
	auto material = ECSManager::GetInstance()->GetComponent<MaterialComponent, true>(id);
	material->front().material.uvTransform = Matrix4x4::MakeAffineMatrix(Vector3(24.0f, 24.0f, 0.0f),
		Vector3::AnyInit(0.0f), Vector3::AnyInit(0.0f));
	material->front().material.shadowRate = 1.0f;
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();
}

void GameScene::ImGui() {
}