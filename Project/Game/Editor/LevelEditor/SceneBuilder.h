#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity3D.h>
#include <Game/Editor/LevelEditor/LevelStructures.h>

// c++
#include <string>
#include <optional>
#include <algorithm>
#include <functional>
// imgui
#include <imgui.h>

//============================================================================
//	SceneBuilder class
//============================================================================
class SceneBuilder {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneBuilder() = default;
	~SceneBuilder() = default;

	void Init(const std::string& jsonPath);

	void CreateEntitiesMap(std::unordered_map<Level::EntityType,
		std::vector<std::unique_ptr<GameEntity3D>>>& entitiesMap);

	void Reset();

	void ImGui();

	//--------- accessor -----------------------------------------------------

	void SetFile(const std::string& sceneFile);

	bool IsCreate() const { return fileName_.has_value(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::string jsonPath_;

	// 追加するときのファイル名
	std::optional<std::string> fileName_;

	// 追加する際に同じ名前なら削除させるかどうかを設定する
	bool idDeleteOnSameName_;

	// editor
	ImVec2 leftChildSize_; // 左側
	ImVec2 buttonSize_;    // ボタンサイズ

	//--------- functions ----------------------------------------------------

	// json
	Json LoadEntityFile(const std::string& identifier);
	void RecieveFile();

	// helper
	void BuildEntities(const Json& obj, std::unordered_map<Level::EntityType,
		std::vector<std::unique_ptr<GameEntity3D>>>& entitiesMap);

	bool IsMeshEntityCreatable(const Json& obj) const;
	template<typename... Ts>
	bool CheckCollisionValid(const GameEntity3D& entity);

	Level::EntityType GetEntityType(const std::string& entityTypeName);

	std::unique_ptr<GameEntity3D> CreateEntity(const Json& obj, Level::EntityType entityType);
	std::unique_ptr<GameEntity3D> CreateEntityPtr(Level::EntityType entityType);

	void HandleDuplicateEntity(std::vector<std::unique_ptr<GameEntity3D>>& entities,
		const std::string& identifier);

	void ApplyTransform(GameEntity3D& entity, const Json& data);
	void ApplyMaterial(GameEntity3D& entity, const Json& data);
	void ApplyCollision(GameEntity3D& entity, const Json& data);
};

//============================================================================
//	SceneBuilder templateMethods
//============================================================================

template<typename... Ts>
inline bool SceneBuilder::CheckCollisionValid(const GameEntity3D& entity) {

	return (... || (dynamic_cast<const Ts*>(&entity) != nullptr));
}