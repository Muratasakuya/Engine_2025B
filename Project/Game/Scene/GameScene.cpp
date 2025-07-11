#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>
#include <Engine/Core/ECS/Core/ECSManager.h>
#include <Engine/Particle/ParticleSystem.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	GameScene classMethods
//============================================================================

void GameScene::Load() {

	//========================================================================
	//	effect
	//========================================================================

	asset_->LoadTexture("circle");
	asset_->LoadTexture("effectCircle");
	asset_->LoadTexture("starCircle");
	asset_->LoadTexture("starCircle2");
	asset_->LoadTexture("circle4");
	asset_->LoadTexture("gradationLine_0");
	asset_->LoadTexture("gradationLine_1");
	asset_->LoadTexture("groundEffect");
	asset_->LoadTexture("redCircle");
	asset_->LoadTexture("horaizontalRedCircle");
	asset_->LoadTexture("verticalRedCircle");
	asset_->LoadTexture("smallCircle");
	asset_->LoadTexture("flashLlight");
	asset_->LoadTexture("noise");
	asset_->LoadTexture("noise1");
	asset_->LoadTexture("noise2");

	asset_->LoadModel("billboardPlane");
	asset_->LoadModel("billboardHorizontalPlane");
	asset_->LoadModel("billboardVerticalPlane");
	asset_->LoadModel("bottomLocalbillboardPlane");
	asset_->LoadModel("topLocalbillboardPlane");
	asset_->LoadModel("crossPlane");
	asset_->LoadModel("groundPlane");
	asset_->LoadModel("groundRing");
	asset_->LoadModel("hitLine");
	asset_->LoadModel("slashEffect");
	asset_->LoadModel("verticalPlane");
	asset_->LoadModel("verticalRing");
	asset_->LoadModel("primitiveRing");
	asset_->LoadModel("primitiveCylinder");

	ParticleSystem::GetInstance()->LoadEmitter("bossTeleportEmitter", "bossTeleportEmitter");
	ParticleSystem::GetInstance()->LoadEmitter("hitEffectEmitter", "hitEffectEmitter");
	ParticleSystem::GetInstance()->LoadEmitter("groundEffectEmitter", "groundEffectEmitter");
	ParticleSystem::GetInstance()->LoadEmitter("bossWarningAttackEmitter", "bossWarningAttackEmitter");
	ParticleSystem::GetInstance()->LoadEmitter("bossHandFireEmitter", "bossHandFireEmitter");

	//========================================================================
	//	environment
	//========================================================================

	// cubeMap、.dds
	asset_->LoadTexture("overcast_soil_puresky_4k");

	// common
	asset_->LoadTexture("whiteAlphaGradation_0");
	asset_->LoadTexture("whiteAlphaGradation_1");

	asset_->LoadModel("field");
	asset_->LoadModel("fieldUnder");
	asset_->LoadModel("fence");
	asset_->LoadModel("wall");
	asset_->LoadModel("antenna");
	asset_->LoadModel("columnMetalSupport");
	asset_->LoadModel("columnPipes");
	asset_->LoadModel("lghtStreet");
	asset_->LoadModel("lightSquare");
	asset_->LoadModel("platformField");
	asset_->LoadModel("platformRound");
	asset_->LoadModel("propCable");
	asset_->LoadModel("propCableLong");
	asset_->LoadModel("propRailOrigin");
	asset_->LoadModel("railLong");
	asset_->LoadModel("signCorner");
	asset_->LoadModel("steelbeams");
	asset_->LoadModel("tiltedWarningWall");
	asset_->LoadModel("wall");
	asset_->LoadModel("warningWall");

	//========================================================================
	//	enemy
	//========================================================================

	// HUD
	asset_->LoadTexture("enemyHPBackground");
	asset_->LoadTexture("enemyHPBar");
	asset_->LoadTexture("enemyDestroyBar");
	asset_->LoadTexture("toughnessNumber");
	asset_->LoadTexture("damageNumber");
	asset_->LoadTexture("timeNumber");
	asset_->LoadTexture("timeSymbol");
	asset_->LoadTexture("bossName");

	// model
	asset_->LoadModel("bossEnemy");
	asset_->LoadModel("bossEnemyWeapon");
	asset_->LoadAnimation("bossEnemy", "bossEnemy");

	asset_->LoadModel("BrainStem");
	asset_->LoadAnimation("BrainStem", "BrainStem");

	//========================================================================
	//	player
	//========================================================================

	// HUD
	asset_->LoadTexture("playerHPBackground");
	asset_->LoadTexture("playerHPBar");
	asset_->LoadTexture("playerSkilBar");
	asset_->LoadTexture("playerName");
	// operate
	asset_->LoadTexture("attackIcon");
	asset_->LoadTexture("dashIcon");
	asset_->LoadTexture("skilIcon");
	asset_->LoadTexture("specialIcon");
	// mouse
	asset_->LoadTexture("leftMouseClick");
	asset_->LoadTexture("rightMouseClick");
	asset_->LoadTexture("centerMouseClick");
	asset_->LoadTexture("EButton");
	asset_->LoadTexture("QButton");
	// gamePad
	asset_->LoadTexture("XButton");
	asset_->LoadTexture("AButton");
	asset_->LoadTexture("YButton");
	asset_->LoadTexture("RTButton");
	asset_->LoadTexture("RBButton");
	asset_->LoadTexture("LBButton");
	// stun
	asset_->LoadTexture("stunProgressBarBackground");
	asset_->LoadTexture("stunProgressBar");
	asset_->LoadTexture("CHAINATTACK");
	asset_->LoadTexture("chainPlayerIcon");
	asset_->LoadTexture("chainPlayerIconRing");
	asset_->LoadTexture("mouseCancel");
	asset_->LoadTexture("gamepadCancel");

	// player
	asset_->LoadModel("player");
	asset_->LoadModel("cube");
	asset_->LoadModel("playerRightWeapon");
	asset_->LoadModel("playerLeftWeapon");
	asset_->LoadAnimation("player", "player");
}

void GameScene::Init() {

	//========================================================================
	//	load
	//========================================================================

	Load();

	//========================================================================
	//	postProcess
	//========================================================================

	postProcessSystem_->Create({ PostProcessType::RadialBlur,PostProcessType::Bloom });
	postProcessSystem_->AddProcess(PostProcessType::RadialBlur);
	postProcessSystem_->AddProcess(PostProcessType::Bloom);

	// ブラーの値を0.0fで初期化
	RadialBlurForGPU radialBlurParam{};
	radialBlurParam.center = Vector2(0.5f, 0.5f);
	radialBlurParam.numSamples = 0;
	radialBlurParam.width = 0.0f;
	postProcessSystem_->SetParameter(radialBlurParam, PostProcessType::RadialBlur);

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

	ECSManager::GetInstance()->CreateSkybox("overcast_soil_puresky_4k");

	viewAnimation_ = std::make_unique<GameEntity3D>();
	viewAnimation_->Init("BrainStem", "BrainStem", "Animation", "BrainStem");

	// 設定
	viewAnimation_->SetScale(Vector3::AnyInit(8.0f));
	viewAnimation_->SetTranslation(Vector3(0.0f, 0.0f, 48.0f));

	//========================================================================
	//	editor
	//========================================================================

	// editor
	entityEditor_ = std::make_unique<GameEntityEditor>();
	entityEditor_->Init(asset_);

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
	// bossEnemy、カメラ、postProcessをセット
	player_->SetBossEnemy(bossEnemy_.get());
	player_->SetFollowCamera(cameraManager_->GetFollowCamera());
	player_->SetPostProcessSystem(postProcessSystem_);
}

void GameScene::Update() {

	//========================================================================
	//	entity
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
	entityEditor_->Update();
	levelEditor_->Update();
}

void GameScene::ImGui() {
}