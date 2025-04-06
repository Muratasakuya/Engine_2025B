#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/InstancedMesh.h>
#include <Engine/Core/Graphics/Mesh/MeshRegistry.h>
#include <Engine/Core/Component/SpriteComponent.h>

// c++
#include <unordered_map>
// entityID
using EntityID = uint32_t;

//============================================================================
//	structure
//============================================================================

// GPUに送るデータ
struct Object2DForGPU {

	DxConstBuffer<Matrix4x4> matrix;
	DxConstBuffer<SpriteMaterial> material;
	SpriteReference sprite;
};

//============================================================================
//	RenderObjectManager class
//============================================================================
class GPUObjectSystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GPUObjectSystem() = default;
	~GPUObjectSystem() = default;

	void Init(ID3D12Device* device, SRVManager* srvManager, Asset* asset);

	void Update();

	//--------- object3D -----------------------------------------------------

	void CreateMesh(const std::string& modelName);

	//--------- object2D -----------------------------------------------------

	// 追加、作成処理
	void CreateObject2D(EntityID id, SpriteComponent* sprite, ID3D12Device* device);
	// 指定されたidのbuffer削除
	void RemoveObject2D(EntityID id);

	//--------- accessor -----------------------------------------------------

	// 3D
	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return instancedMesh_->GetInstancingData(); }

	// meshの取得
	Mesh* GetMesh(const std::string& name) const { return meshRegistry_->GetMesh(name); }
	const std::unordered_map<std::string, std::unique_ptr<Mesh>>& GetMeshes() const { return meshRegistry_->GetMeshes(); }

	// 2D
	const std::vector<Object2DForGPU>& GetObject2DBuffers() const { return object2DBuffers_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 3D
	std::unique_ptr<InstancedMesh> instancedMesh_;
	std::unique_ptr<MeshRegistry> meshRegistry_;

	// 2D
	std::vector<Object2DForGPU> object2DBuffers_;

	// buffer管理
	// 2D
	std::unordered_map<EntityID, size_t> object2DBufferToIndex_;
	std::vector<EntityID> indexToObject2DBuffer_;

	//--------- functions ----------------------------------------------------

	// bufferの更新処理
	// 3D
	void UpdateObject3D();
	// 2D
	void UpdateObject2D();
};