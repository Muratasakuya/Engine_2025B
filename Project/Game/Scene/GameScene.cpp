#include "GameScene.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBufferSize.h>
#include <Engine/Object/Core/ObjectManager.h>
#include <Engine/Scene/SceneView.h>
#include <Engine/Asset/Asset.h>

// scene
#include <Game/Scene/GameState/States/StartGameState.h>
#include <Game/Scene/GameState/States/BeginGameState.h>
#include <Game/Scene/GameState/States/PlayGameState.h>
#include <Game/Scene/GameState/States/EndGameState.h>
#include <Game/Scene/GameState/States/PauseState.h>

//============================================================================
//	GameScene classMethods
//============================================================================

void GameScene::InitStates() {

	// scene
	context_.camera = cameraManager_.get();
	context_.light = gameLight_.get();
	context_.fieldBoundary = fieldBoundary_.get();
	// object
	context_.player = player_.get();
	context_.boss = bossEnemy_.get();
	// editor
	context_.level = levelEditor_.get();

	// シーン状態クラスの初期化
	states_[static_cast<uint32_t>(State::Start)] = std::make_unique<StartGameState>(&context_);
	states_[static_cast<uint32_t>(State::Start)]->Init(sceneView_);

	states_[static_cast<uint32_t>(State::BeginGame)] = std::make_unique<BeginGameState>(&context_);
	states_[static_cast<uint32_t>(State::BeginGame)]->Init(sceneView_);

	states_[static_cast<uint32_t>(State::PlayGame)] = std::make_unique<PlayGameState>(&context_);
	states_[static_cast<uint32_t>(State::PlayGame)]->Init(sceneView_);

	states_[static_cast<uint32_t>(State::EndGame)] = std::make_unique<EndGameState>(&context_);
	states_[static_cast<uint32_t>(State::EndGame)]->Init(sceneView_);

	states_[static_cast<uint32_t>(State::Pause)] = std::make_unique<PauseState>(&context_);
	states_[static_cast<uint32_t>(State::Pause)]->Init(sceneView_);

	// 最初の状態を設定
	currentState_ = State::Start;
}

void GameScene::Init() {

	//========================================================================
	//	sceneObjects
	//========================================================================

	cameraManager_ = std::make_unique<CameraManager>();
	gameLight_ = std::make_unique<PunctualLight>();
	fieldBoundary_ = std::make_unique<FieldBoundary>();

	//========================================================================
	//	editor
	//========================================================================

	levelEditor_ = std::make_unique<LevelEditor>();

	//========================================================================
	//	frontObjects
	//========================================================================

	player_ = std::make_unique<Player>();
	bossEnemy_ = std::make_unique<BossEnemy>();

	//========================================================================
	//	state
	//========================================================================

	// シーン状態の初期化
	InitStates();
}

void GameScene::Update() {

	// 状態に応じて更新
	uint32_t stateIndex = static_cast<uint32_t>(currentState_);
	switch (currentState_) {
		//========================================================================
		//	ゲーム開始時の処理
		//========================================================================
	case GameScene::State::Start: {

		states_[stateIndex]->Update(nullptr);
		break;
	}
		//========================================================================
		//	ゲーム開始演出の処理
		//========================================================================
	case GameScene::State::BeginGame: {

		states_[stateIndex]->Update(nullptr);
		break;
	}
		//========================================================================
		//	ゲームプレイ中の処理
		//========================================================================
	case GameScene::State::PlayGame: {

		states_[stateIndex]->Update(nullptr);
		break;
	}
		//========================================================================
		//	ゲーム終了時の処理
		//========================================================================
	case GameScene::State::EndGame: {

		states_[stateIndex]->Update(nullptr);
		break;
	}
		//========================================================================
		//	ポーズ中の処理
		//========================================================================
	case GameScene::State::Pause: {

		states_[stateIndex]->Update(nullptr);
		break;
	}
	}
}

void GameScene::ImGui() {
}