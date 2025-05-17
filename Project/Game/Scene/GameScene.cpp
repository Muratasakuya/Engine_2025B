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

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Load(Asset* asset) {

	// debug
	asset->LoadModel("sphere");

	// player
	asset->LoadModel("playerBody");
	asset->LoadModel("playerLeftHand");
	asset->LoadModel("playerRightHand");
	asset->LoadModel("playerSword");

	// effect
	asset->LoadTexture("effectCircle");
	asset->LoadTexture("sirialHitEffect");
	asset->LoadTexture("sirialLightning_0");
	asset->LoadTexture("sirialLightning_1");

	// cubeMap、.dds
	asset->LoadTexture("docklands_01_2k");

	// environment
	asset->LoadModel("stageField");

	// primitive
	asset->LoadModel("sirialHitEffectPlane");
	asset->LoadModel("effectDefaultPlane");
	asset->LoadModel("sirialLightningEffectPlane");

	// debugAnimation
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

	uint32_t id = ECSManager::GetInstance()->CreateObject3D("BrainStem", "BrainStem", "Human", "BrainStem");
	auto* animation = ECSManager::GetInstance()->GetComponent<AnimationComponent>(id);
	animation->SetAnimationData("BrainStem");
	animation->SetPlayAnimation("BrainStem", true);

	ECSManager::GetInstance()->CreateObject3D("sphere", "sphere", "Sphere");
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();

	// grid描画
	LineRenderer::GetInstance()->DrawGrid(32, 128.0f, Color::White());
}

void GameScene::ImGui() {
}