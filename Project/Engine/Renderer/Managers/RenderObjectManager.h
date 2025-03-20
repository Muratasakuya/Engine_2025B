#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Mesh/InstancedMesh.h>
#include <Engine/Component/SpriteComponent.h>

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

struct Object2DForGPU {

	DxConstBuffer<Matrix4x4> matrix;
	DxConstBuffer<SpriteMaterial> material;
	SpriteReference sprite;
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

	void Update();

	//--------- object3D -----------------------------------------------------

	// 追加、作成処理
	void CreateObject3D(EntityID id, const std::optional<std::string>& instancingName,
		ModelComponent* model, ID3D12Device* device);
	// 指定されたidのbuffer削除
	void RemoveObject3D(EntityID id);

	//--------- object2D -----------------------------------------------------

	// 追加、作成処理
	void CreateObject2D(EntityID id, SpriteComponent* sprite, ID3D12Device* device);
	// 指定されたidのbuffer削除
	void RemoveObject2D(EntityID id);

	//--------- accessor -----------------------------------------------------

	// 3D
	const std::vector<Object3DForGPU>& GetObject3DBuffers() const { return object3DBuffers_; }
	const std::unordered_map<std::string, InstancingData>& GetInstancingData() const { return instancedMesh_->GetData(); }

	// 2D
	const std::vector<Object2DForGPU>& GetObject2DBuffers() const { return object2DBuffers_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 3D
	// 通常描画処理用
	std::vector<Object3DForGPU> object3DBuffers_;
	// instancing描画用
	std::unique_ptr<InstancedMesh> instancedMesh_;

	// 2D
	std::vector<Object2DForGPU> object2DBuffers_;

	//--------- functions ----------------------------------------------------

	// bufferの更新処理
	// 3D
	void UpdateObject3D();
	// 2D
	void UpdateObject2D();
};