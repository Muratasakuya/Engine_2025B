#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>
#include <Engine/Asset/AssetStructure.h>

// assimp
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
// meshoptimizer
#include <meshoptimizer.h>

//============================================================================
//	MeshletBuilder class
//============================================================================
class MeshletBuilder {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshletBuilder() = default;
	~MeshletBuilder() = default;

	ResourceMesh ParseMesh(const aiScene* scene, bool isSkinned);
	ResourceMesh ParseMesh(const ModelData& modelData);

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// 頂点設定
	void SetVertex(ResourceMesh& destinationMesh, const aiScene* scene);
	void SetVertex(ResourceMesh& destinationMesh, const ModelData& modelData);

	// 最適化処理
	void Optimize(ResourceMesh& destinationMesh);

	// meshlet生成
	void CreateMeshlet(ResourceMesh& destinationMesh);
};