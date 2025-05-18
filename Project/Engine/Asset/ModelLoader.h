#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>

// c++
#include <unordered_map>
// front
class TextureManager;

//============================================================================
//	ModelManager class
//============================================================================
class ModelLoader {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ModelLoader() = default;
	~ModelLoader() = default;

	void Init(TextureManager* textureManager);

	void Load(const std::string& modelName);

	void Make(const std::string& modelName,
		const std::vector<MeshVertex>& vertexData,
		const std::vector<uint32_t>& indexData);
	void Export(const ResourceMesh& modelData, const std::string& filePath);

	bool Search(const std::string& modelName);

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