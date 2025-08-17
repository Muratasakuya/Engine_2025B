#include "StartGameState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Scene/SceneView.h>

//============================================================================
//	StartGameState classMethods
//============================================================================

void StartGameState::Init(SceneView* sceneView) {

	// ゲーム開始時、全ての初期化を行う

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

	// カメラ
	context_->camera->Init(sceneView);

	// ライトの初期設定
	context_->light->Init();
	context_->light->directional.direction.x = 0.558f;
	context_->light->directional.direction.y = -0.476f;
	context_->light->directional.direction.z = -0.68f;
	context_->light->directional.color = Color::Convert(0xadceffff);
	sceneView->SetLight(context_->light);

	// 衝突
	context_->fieldCollision->Init();

	//========================================================================
	//	backObjects
	//========================================================================

	ObjectManager::GetInstance()->CreateSkybox("overcast_soil_puresky_4k");

	//========================================================================
	//	editor
	//========================================================================

	context_->level->Init("levelEditor");

	//========================================================================
	//	frontObjects
	//========================================================================

	// プレイヤー
	context_->player->Init("player", "player", "Player", "player_idle");

	// 追従先を設定する: player
	context_->camera->SetTarget(context_->player);

	// ボス
	context_->boss->Init("bossEnemy", "bossEnemy", "Enemy", "bossEnemy_idle");

	// プレイヤー、カメラをセット
	context_->boss->SetPlayer(context_->player);
	context_->boss->SetFollowCamera(context_->camera->GetFollowCamera());
	// ボス、カメラをセット
	context_->player->SetBossEnemy(context_->boss);
	context_->player->SetFollowCamera(context_->camera->GetFollowCamera());

	// 衝突応答にプレイヤー、ボスをセット
	context_->fieldCollision->SetPushBackTarget(context_->player, context_->boss);
}

void StartGameState::Update([[maybe_unused]] SceneManager* sceneManager) {

	//========================================================================
	//	object
	//========================================================================

	context_->player->Update();
	context_->boss->Update();

	//========================================================================
	//	scene
	//========================================================================

	context_->camera->Update();
	context_->level->Update();
	context_->fieldCollision->Update();
}

void StartGameState::NonActiveUpdate([[maybe_unused]] SceneManager* sceneManager) {
}