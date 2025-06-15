#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Editor/LevelEditor/SceneBuilder.h>

// c++
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
// front
class Asset;

//============================================================================
//	LevelEditor class
//============================================================================
class LevelEditor :
	IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LevelEditor() :IGameEditor("LevelEditor") {};
	~LevelEditor() = default;

	void Init();

	void Update();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 全てのentityを管理
	std::unordered_map<Level::EntityType, std::vector<std::unique_ptr<GameEntity3D>>> entitiesMap_;

	// builder
	std::unique_ptr<SceneBuilder> sceneBuilder_;

	//--------- functions ----------------------------------------------------

	void BuildEntities();

	void UpdateEntities();
};