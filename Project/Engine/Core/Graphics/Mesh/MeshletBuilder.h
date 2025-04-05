#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/MeshletStructures.h>

// directX
#include <Externals/DirectXMesh/include/DirectXMesh.h>
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

	ResourceMesh ParseMesh(const aiMesh* sourceMesh);

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	// 頂点設定
	void SetVertex(ResourceMesh& destinationMesh, const aiMesh* sourceMesh);

	// 最適化処理
	void Optimize(ResourceMesh& destinationMesh);

	// meshlet生成
	void CreateMeshlet(ResourceMesh& destinationMesh);
};