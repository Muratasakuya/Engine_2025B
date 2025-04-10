#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Core/Component/TransformComponent.h>
#include <Engine/Core/Component/MaterialComponent.h>
#include <Engine/Core/Component/Base/IComponent.h>
#include <Engine/Core/Graphics/Mesh/Mesh.h>

//============================================================================
//	structure
//============================================================================

struct MeshInstancingData {

	// instance数
	uint32_t maxInstance;
	uint32_t numInstance;

	// buffer更新用のデータ
	std::vector<TransformationMatrix> matrixUploadData;
	std::vector<std::vector<Material>> materialUploadData;

	// buffer
	DxConstBuffer<TransformationMatrix> matrix;
	std::vector<DxConstBuffer<Material>> materials;

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

	void Create(class Mesh* mesh, const std::string& name, uint32_t numInstance);

	void Update();

	void Reset();

	//--------- accessor -----------------------------------------------------

	void SetDevice(ID3D12Device* device);

	void SetUploadData(const std::string& name, const TransformationMatrix& matrix,
		const std::vector<MaterialComponent>& materials);

	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return meshGroups_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;

	std::unordered_map<std::string, MeshInstancingData> meshGroups_;

	//--------- functions ----------------------------------------------------

	void CreateBuffers(const std::string& name);
};