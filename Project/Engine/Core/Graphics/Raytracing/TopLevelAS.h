#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Raytracing/AccelerationStructureBuffer.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingStructures.h>

//============================================================================
//	TopLevelAS class
//============================================================================
class TopLevelAS {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TopLevelAS() = default;
	~TopLevelAS() = default;

	void Build(ID3D12Device8* device, ID3D12GraphicsCommandList6* commandList,
		const std::vector<RayTracingInstance>& instances, bool allowUpdate);

	void Update(ID3D12GraphicsCommandList6* commandList,
		const std::vector<RayTracingInstance>& instances);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetResource() const { return result_.GetResource(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// スクラッチリソース
	AccelerationStructureBuffer instanceDescs_;
	AccelerationStructureBuffer scratch_;
	AccelerationStructureBuffer result_;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs_;
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC  buildDesc_;

	bool allowUpdate_;

	//--------- functions ----------------------------------------------------

	void CopyMatrix3x4(float(&dst)[3][4], const Matrix4x4& src);
};