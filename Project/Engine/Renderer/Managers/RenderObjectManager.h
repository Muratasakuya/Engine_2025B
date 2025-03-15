#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/CBuffer/DxConstBuffer.h>
#include <Engine/Core/Lib/CBufferStructures.h>
#include <Engine/Component/ModelComponent.h>

// entityID
using EntityID = uint32_t;

//============================================================================
//	RenderObjectManager class
//============================================================================
class RenderObjectManager {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// GPUに送るデータ
	struct Object3DForGPU {

		DxConstBuffer<TransformationMatrix> matrix;
		std::vector<DxConstBuffer<Material>> materials;
		ModelReference model;
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RenderObjectManager() = default;
	~RenderObjectManager() = default;

	void CreateObject3D(EntityID id, ModelComponent* model, ID3D12Device* device);
	void RemoveObject3D(EntityID id);

	void Update();

	//--------- accessor -----------------------------------------------------

	std::vector<Object3DForGPU> GetObject3DBuffers() { return object3DBuffers_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::vector<Object3DForGPU> object3DBuffers_;
};