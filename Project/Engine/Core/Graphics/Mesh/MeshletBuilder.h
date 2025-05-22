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

	ResourceMesh<MeshVertex> ParseMesh(const aiScene* scene, bool isSkinned);
	ResourceMesh<MeshVertex> ParseMesh(const ModelData& modelData);
	ResourceMesh<EffectMeshVertex> ParseEffectMesh(const ModelData& modelData);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// 頂点設定
	void SetVertex(ResourceMesh<MeshVertex>& destinationMesh, const aiScene* scene);
	void SetVertex(ResourceMesh<MeshVertex>& destinationMesh, const ModelData& modelData);
	void SetEffectVertex(ResourceMesh<EffectMeshVertex>& destinationMesh, const ModelData& modelData);

	// 最適化処理
	void Optimize(ResourceMesh<MeshVertex>& destinationMesh);
	void OptimizeEffectMesh(ResourceMesh<EffectMeshVertex>& destinationMesh);

	// meshlet生成
	void CreateMeshlet(ResourceMesh<MeshVertex>& destinationMesh);
	void CreateEffectMeshlet(ResourceMesh<EffectMeshVertex>& destinationMesh);
};