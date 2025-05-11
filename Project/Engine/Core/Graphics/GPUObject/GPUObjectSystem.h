#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/GPUObject/InstancedMeshBuffer.h>
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>
#include <Engine/Core/Graphics/Mesh/MeshRegistry.h>
#include <Engine/Core/Component/AnimationComponent.h>
#include <Engine/Core/Component/PrimitiveMeshComponent.h>
#include <Engine/Core/Component/SpriteComponent.h>

// c++
#include <unordered_map>

//============================================================================
//	structure
//============================================================================

// GPUに送るデータ
struct EffectForGPU {

	DxConstBuffer<TransformationMatrix> matrix;
	DxConstBuffer<EffectMaterial> material;
	PrimitiveMesh* primitiveMesh;
};

struct Object2DForGPU {

	DxConstBuffer<Matrix4x4> matrix;
	DxConstBuffer<SpriteMaterial> material;
	SpriteComponent* sprite;
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

	void Init(ID3D12Device* device, Asset* asset);

	void Update(class CameraManager* cameraManager,
		class LightManager* lightManager,
		class DxCommand* dxCommand);

	//--------- object3D -----------------------------------------------------

	void CreateStaticMesh(const std::string& modelName);
	void CreateSkinnedMesh(const std::string& modelName);

	//---------- effect ------------------------------------------------------

	// 追加、作成処理
	void CreateEffect(uint32_t entityId, PrimitiveMeshComponent* primitiveMesh, ID3D12Device* device);
	// 指定されたidのbuffer削除
	void RemoveEffect(uint32_t entityId);

	//--------- object2D -----------------------------------------------------

	// 追加、作成処理
	void CreateObject2D(uint32_t entityId, SpriteComponent* sprite, ID3D12Device* device);
	// 指定されたidのbuffer削除
	void RemoveObject2D(uint32_t entityId);

	//--------- accessor -----------------------------------------------------

	// scene
	SceneConstBuffer* GetSceneBuffer() const { return sceneConstBuffer_.get(); }

	// 3D
	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return instancedMeshBuffer_->GetInstancingData(); }

	// meshの取得
	IMesh* GetMesh(const std::string& name) const { return meshRegistry_->GetMesh(name); }
	const std::unordered_map<std::string, std::unique_ptr<IMesh>>& GetMeshes() const { return meshRegistry_->GetMeshes(); }

	// effect
	const std::vector<EffectForGPU>& GetEffectBuffers() const { return effectBuffers_; }

	// 2D
	const std::vector<Object2DForGPU>& GetObject2DBuffers() const { return object2DBuffers_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// scene
	std::unique_ptr<SceneConstBuffer> sceneConstBuffer_;

	// 3D
	std::unique_ptr<InstancedMeshBuffer> instancedMeshBuffer_;
	std::unique_ptr<MeshRegistry> meshRegistry_;

	// 2D
	std::vector<Object2DForGPU> object2DBuffers_;

	// effect
	std::vector<EffectForGPU> effectBuffers_;

	// buffer管理
	// 2D
	std::unordered_map<uint32_t, size_t> object2DBufferToIndex_;
	std::vector<uint32_t> indexToObject2DBuffer_;

	// effect
	std::unordered_map<uint32_t, size_t> effectBufferToIndex_;
	std::vector<uint32_t> indexToEffectBuffer_;

	// debug
	bool isUploadBufer_ = false;

	//--------- functions ----------------------------------------------------

	void SwapToPopbackObject2D(uint32_t entityId);
	void SwapToPopbackEffect(uint32_t entityId);

	// bufferの更新処理
	// 3D
	void UpdateObject3D(class DxCommand* dxCommand);
	// effect
	void UpdateEffect();
	// 2D
	void UpdateObject2D();
};