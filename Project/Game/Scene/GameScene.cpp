#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Core/ECS/Components/AnimationComponent.h>
#include <Engine/Particle/ParticleSystem.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

void GameScene::Load(Asset* asset) {

	// cubeMap、.dds
	asset->LoadTexture("docklands_01_2k");
	// lut
	asset->LoadLutTexture("lut");

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
	postProcessSystem_->Create({ 
		PostProcessType::Bloom,
		PostProcessType::Lut });

	postProcessSystem_->AddProcess(PostProcessType::Bloom);
	postProcessSystem_->AddProcess(PostProcessType::Lut);

	postProcessSystem_->InputProcessTexture("lut", PostProcessType::Lut, asset);

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

	// entityEditor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset);
	// levelEditor
	levelEditor_ = std::make_unique<LevelEditor>();
	levelEditor_->LoadFile(asset);

	// 仮の地面
	uint32_t id = ECSManager::GetInstance()->CreateObject3D("stageField", "field", "Environment");
	auto transform = ECSManager::GetInstance()->GetComponent<Transform3DComponent>(id);
	transform->translation.y = -0.8f;
	transform->scale.x = 128.0f;
	transform->scale.z = 128.0f;
	auto material = ECSManager::GetInstance()->GetComponent<MaterialComponent, true>(id);
	material->front().uvMatrix = Matrix4x4::MakeAffineMatrix(Vector3(24.0f, 24.0f, 0.0f),
		Vector3::AnyInit(0.0f), Vector3::AnyInit(0.0f));

	id = ECSManager::GetInstance()->CreateObject3D("BrainStem", "BrainStem", "BrainStem", "BrainStem");
	auto anim = ECSManager::GetInstance()->GetComponent<AnimationComponent>(id);
	anim->SetPlayAnimation("BrainStem", true);
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();

	// entityEditor
	entityEditor_->Update();
}

void GameScene::ImGui() {
}