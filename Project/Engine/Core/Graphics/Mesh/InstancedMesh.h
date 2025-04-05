#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/CBuffer/DxConstBuffer.h>
#include <Engine/Core/Graphics/CBuffer/CBufferStructures.h>
#include <Engine/Core/Graphics/Mesh/Mesh.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Component/Base/IComponent.h>

// front
class SRVManager;

//============================================================================
//	structure
//============================================================================

struct MeshInstancingData {

	// srv処理用
	uint32_t matrixSrvIndex;
	std::vector<uint32_t> materialSrvIndices;

	// instance数
	uint32_t maxInstance;
	uint32_t numInstance;

	// buffer更新用のデータ
	std::vector<TransformationMatrix> matrixComponents;
	std::vector<std::vector<Material>> materialComponents;

	// buffer
	DxConstBuffer<TransformationMatrix> matrix;
	std::vector<DxConstBuffer<Material>> materials;

	size_t meshNum;
};

//============================================================================
//	InstancedMesh class
//============================================================================
class InstancedMesh {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	InstancedMesh() = default;
	~InstancedMesh() = default;

	void Init(ID3D12Device* device, SRVManager* srvManager);

	void Create(class Mesh* mesh, const std::string& name, uint32_t numInstance);

	void Update();

	void Reset();

	//--------- accessor -----------------------------------------------------

	void SetComponent(const std::string& name, const TransformationMatrix& matrix,
		const std::vector<Material>& materials);

	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return meshGroups_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	SRVManager* srvManager_;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvBufferDesc_;

	std::unordered_map<std::string, MeshInstancingData> meshGroups_;

	//--------- functions ----------------------------------------------------

	void CreateBuffers(const std::string& name);
};