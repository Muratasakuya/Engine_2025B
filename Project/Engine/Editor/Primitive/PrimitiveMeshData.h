#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>

// front
class Asset;

//============================================================================
//	PrimitiveMeshData class
//============================================================================
class PrimitiveMeshData {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PrimitiveMeshData() = default;
	~PrimitiveMeshData() = default;

	void Create(Asset* asset);

	//--------- accessor -----------------------------------------------------

	const std::vector<std::string>& GetNames() const { return meshNames_; }

	const MeshModelData& GetMeshData(const std::string& name) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Asset* asset_;

	// primitiveMeshの名前
	std::vector<std::string> meshNames_;

	// string型で保持するモデルの頂点情報
	std::unordered_map<std::string, MeshModelData> meshData_;

	//--------- functions ----------------------------------------------------

	void CreatePlane();
};