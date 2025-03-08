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
		ModelComponent* model;
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

	const std::unordered_map<BlendMode, std::vector<const Object3DForGPU*>>&
		GetSortedObject3Ds() const;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unordered_map<EntityID, Object3DForGPU> object3DBuffers_;

	// blendModeごとのObject
	mutable bool needsSorting_ = true;
	mutable std::unordered_map<BlendMode, std::vector<const Object3DForGPU*>> sortedObject3Ds_;

	//--------- functions ----------------------------------------------------

	void RebuildBlendModeCache() const;
};