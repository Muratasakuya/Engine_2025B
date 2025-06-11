#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/ECS/Components/TransformComponent.h>

// c++
#include <string>
#include <istream>
#include <cassert>
// front
class Asset;

//============================================================================
//	LevelEditor class
//============================================================================
class LevelEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	LevelEditor() = default;
	~LevelEditor() = default;

	void LoadFile(Asset* asset);

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct LevelData {

		std::string modelName;
		Transform3DComponent transform;
	};

	//--------- variables ----------------------------------------------------

	std::unordered_map<std::string, std::vector<LevelData>> lebelDataMap_;

	//--------- functions ----------------------------------------------------

};