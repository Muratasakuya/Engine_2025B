#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Raytracing/AccelerationStructureBuffer.h>

// c++
#include <vector>
// front
class IMesh;

//============================================================================
//	structure
//============================================================================

struct BuildRequest {

	IMesh* mesh;
	uint32_t meshIndex;
	bool allowUpdate; // skinnedMesh == true
};

//============================================================================
//	BottomLevelAS class
//============================================================================
class BottomLevelAS {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BottomLevelAS() = default;
	~BottomLevelAS() = default;

	void Build(ID3D12Device8* device, ID3D12GraphicsCommandList6* commandList,
		const BuildRequest& request);
	
	// skinnedMeshの更新
	void Update(ID3D12GraphicsCommandList6* commandList);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetResource() const { return result_.GetResource(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// スクラッチリソース
	AccelerationStructureBuffer scratch_;
	AccelerationStructureBuffer result_;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs_;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC  buildDesc_;

	bool allowUpdate_;
};