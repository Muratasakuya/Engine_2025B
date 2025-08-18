#pragma once

//============================================================================
//	include
//============================================================================

// scene
#include <Game/Camera/Manager/CameraManager.h>
#include <Engine/Scene/Light/PunctualLight.h>
#include <Game/Objects/GameScene/Environment/Collision/FieldBoundary.h>

// object
#include <Game/Objects/GameScene/Player/Entity/Player.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

// sprite
#include <Game/Objects/GameScene/SpriteEffect/FadeSprite.h>

// editor
#include <Game/Editor/LevelEditor/LevelEditor.h>

//============================================================================
//	GameContext
//============================================================================

// シーン内で動かすもの
struct GameContext {

	// scene
	CameraManager* camera = nullptr;
	PunctualLight* light = nullptr;
	FieldBoundary* fieldBoundary = nullptr;

	// object
	Player* player = nullptr;
	BossEnemy* boss = nullptr;
	
	// sprite
	FadeSprite* fadeSprite = nullptr;

	// editor
	LevelEditor* level = nullptr;
};