#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>

// c++
#include <unordered_map>
// front
class TextureManager;

//============================================================================
//	ModelManager class
//============================================================================
class ModelManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelManager() = default;
	~ModelManager() = default;

	void Init(TextureManager* textureManager);

	void Load(const std::string& modelName);

	//--------- accessor -----------------------------------------------------

	const ModelData& GetModelData(const std::string& modelName) const;

	const std::vector<std::string>& GetModelKeys() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	TextureManager* textureManager_;

	std::string baseDirectoryPath_;

	std::unordered_map<std::string, ModelData> models_;

	mutable std::vector<std::string> modelKeysCache_;
	mutable bool isCacheValid_;

	//--------- functions ----------------------------------------------------

	ModelData LoadModelFile(const std::string& filePath);

	Node ReadNode(aiNode* node);
};