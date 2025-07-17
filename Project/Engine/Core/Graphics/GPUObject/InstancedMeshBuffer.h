#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Object/Data/Transform.h>
#include <Engine/Object/Data/Material.h>
#include <Engine/Object/Data/Animation.h>
#include <Engine/Core/Graphics/Mesh/Mesh.h>

// front
class Asset;

//============================================================================
//	structure
//============================================================================

struct MeshInstancingData {

	// instance数
	uint32_t maxInstance;
	uint32_t numInstance;

	// staticかskinnedかのフラグ
	bool isSkinned;

	// buffer更新用のデータ
	std::vector<TransformationMatrix> matrixUploadData;
	std::vector<std::vector<MaterialForGPU>> materialUploadData;
	std::vector<std::vector<LightingForGPU>> lightingUploadData;
	// skinnedMeshのbuffer更新用のデータ
	std::vector<std::vector<WellForGPU>> wellUploadData;

	// buffer
	DxConstBuffer<TransformationMatrix> matrixBuffer;
	std::vector<DxConstBuffer<MaterialForGPU>> materialsBuffer;
	std::vector<DxConstBuffer<LightingForGPU>> lightingBuffer;
	// skinnedMesh専用buffer
	std::vector<DxConstBuffer<WellForGPU>> wells;
	std::vector<DxConstBuffer<VertexInfluence>> influences;
	std::vector<DxConstBuffer<SkinningInformation>> skinningInformations;
	// mesh情報
	IMesh* skinnedMesh;
	std::vector<UINT> vertexSizes;
	UINT boneSize;

	// meshの数
	size_t meshNum;
};

//============================================================================
//	InstancedMeshBuffer class
//============================================================================
class InstancedMeshBuffer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	InstancedMeshBuffer() = default;
	~InstancedMeshBuffer() = default;

	void Init(ID3D12Device* device, Asset* asset);

	void Create(class IMesh* mesh, const std::string& name, uint32_t numInstance);

	void Update(class DxCommand* dxCommand);

	void Reset();

	//--------- accessor -----------------------------------------------------

	void SetUploadData(const std::string& name, const TransformationMatrix& matrix,
		const std::vector<Material>& materials,
		const SkinnedAnimation& animation);

	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return meshGroups_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;

	std::unordered_map<std::string, MeshInstancingData> meshGroups_;

	//--------- functions ----------------------------------------------------

	void CreateBuffers(const std::string& name);
	void CreateSkinnedMeshBuffers(const std::string& name);
};