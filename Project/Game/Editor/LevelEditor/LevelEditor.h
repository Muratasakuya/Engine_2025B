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

	void Init(const std::optional<std::string>& initSceneFile);

	void Update();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const std::string jsonPath_ = "Level/EntityData/";

	// 全てのentityを管理
	std::unordered_map<Level::EntityType, std::vector<std::unique_ptr<GameEntity3D>>> entitiesMap_;

	// builder
	std::unique_ptr<SceneBuilder> sceneBuilder_;

	Level::EntityType currentSelectType_;   // 選択中のタイプ
	std::optional<int> currentSelectIndex_; // 選択インデックス

	// editor
	ImVec2 rightChildSize_; // 右側
	ImVec2 buttonSize_;    // ボタンサイズ

	//--------- functions ----------------------------------------------------

	// json
	void SaveEntity(GameEntity3D* entity);

	// update
	void BuildEntities();
	void UpdateEntities();

	// editor
	void SelectEntity();
	void EditEntity();
};