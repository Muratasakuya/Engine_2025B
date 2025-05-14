#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Core/Graphics/Skybox/Skybox.h>
#include <Engine/Renderer/LineRenderer.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Engine/Scene/Light/LightManager.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	TitleScene classMethods
//============================================================================

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

	//object3DEditor_ = std::make_unique<Object3DEditor>();
	//object3DEditor_->Init(asset);

	player_ = std::make_unique<Player>();
	player_->Init(followCamera_.get());

	// 追従先を設定する: player
	followCamera_->SetTarget(player_->GetTransform());

	// skybox
	Skybox::GetInstance()->Create(asset->GetTextureGPUIndex("docklands_01_2k"));
}

void GameScene::Update([[maybe_unused]] SceneManager* sceneManager) {

	followCamera_->Update();

	player_->Update();

	// grid描画
	LineRenderer::GetInstance()->DrawGrid(32, 128.0f, Color::White());
}

void GameScene::Load(Asset* asset) {

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

	const uint32_t kNumObject = 8;
	const float offset = 4.0f;
	const uint32_t gridSize = static_cast<uint32_t>(std::sqrt(kNumObject));

	for (uint32_t index = 0; index < kNumObject; ++index) {

		debugAnimationContoroller_.push_back(false);
		debugId_.push_back(GameObjectHelper::CreateObject3D(
			"BrainStem", "BrainStem", "Human", "BrainStem"));
		auto transform = Component::GetComponent<Transform3DComponent>(debugId_[index]);

		auto animation = Component::GetComponent<AnimationComponent>(debugId_[index]);
		animation->SetAnimationData("BrainStem");

		uint32_t x = index % gridSize;
		uint32_t z = index / gridSize;

		transform->translation.x = x * offset;
		transform->translation.z = z * offset;
	}
}

void GameScene::ImGui() {

	for (uint32_t i = 0; i < debugId_.size(); ++i) {

		bool debugAnim = debugAnimationContoroller_[i];
		if (ImGui::Checkbox(("playAnimation" + std::to_string(i)).c_str(),
			&debugAnim)) {

			debugAnimationContoroller_[i] = debugAnim;

			auto animation = Component::GetComponent<AnimationComponent>(debugId_[i]);
			animation->SetPlayAnimation("BrainStem", debugAnim);
		}
	}
}