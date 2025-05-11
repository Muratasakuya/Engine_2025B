#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/Mesh.h>

// c++
#include <string>
#include <unordered_map>
// front
class Asset;

//============================================================================
//	MeshRegistry class
//============================================================================
class MeshRegistry {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	MeshRegistry() = default;
	~MeshRegistry() = default;

	void Init(ID3D12Device* device, Asset* asset);

	void RegisterMesh(const std::string& modelName,
		bool isSkinned, uint32_t numInstance);

	//--------- accessor -----------------------------------------------------

	// meshの取得
	IMesh* GetMesh(const std::string& name) const { return meshes_.at(name).get(); }
	const std::unordered_map<std::string, std::unique_ptr<IMesh>>& GetMeshes() const { return meshes_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;

	std::unordered_map<std::string, std::unique_ptr<IMesh>> meshes_;

	//--------- functions ----------------------------------------------------

	// meshletの作成
	ResourceMesh CreateMeshlet(const std::string& modelName);
};