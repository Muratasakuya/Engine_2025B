#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/InstancedMesh.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	structure
//============================================================================

// GPUに送るデータ
struct Object3DForGPU {

	DxConstBuffer<TransformationMatrix> matrix;
	std::vector<DxConstBuffer<Material>> materials;
	ModelReference model;
};

//============================================================================
//	RenderObjectManager class
//============================================================================
class RenderObjectManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RenderObjectManager() = default;
	~RenderObjectManager() = default;

	void Init(ID3D12Device* device, SRVManager* srvManager);

	void CreateObject3D(EntityID id, const std::optional<std::string>& instancingName,
		ModelComponent* model, ID3D12Device* device);

	void RemoveObject3D(EntityID id);

	void Update();

	//--------- accessor -----------------------------------------------------

	std::vector<Object3DForGPU> GetObject3DBuffers() { return object3DBuffers_; }

	const std::unordered_map<std::string, InstancingData>& GetInstancingData() const { return instancedMesh_->GetData(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 通常描画処理用
	std::vector<Object3DForGPU> object3DBuffers_;

	// instancing描画用
	std::unique_ptr<InstancedMesh> instancedMesh_;
};