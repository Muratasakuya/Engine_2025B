#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/System/Base/ISystem.h>
#include <Engine/Core/Graphics/Mesh/MeshRegistry.h>
#include <Engine/Core/Graphics/GPUObject/InstancedMeshBuffer.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingStructures.h>

// directX
#include <d3d12.h>
// front
class DxCommand;
class Asset;
class RaytracingScene;

//============================================================================
//	InstancedMeshSystem class
//============================================================================
class InstancedMeshSystem :
	public ISystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	InstancedMeshSystem(ID3D12Device* device,
		Asset* asset, DxCommand* dxCommand);
	~InstancedMeshSystem() = default;

	// createBuffer
	void CreateStaticMesh(const std::string& modelName);
	void CreateSkinnedMesh(const std::string& modelName);

	Archetype Signature() const override;

	void Update(ObjectPoolManager& ObjectPoolManager) override;

	//--------- accessor -----------------------------------------------------

	const std::unordered_map<std::string, std::unique_ptr<IMesh>>& GetMeshes() const { return meshRegistry_->GetMeshes(); }
	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return instancedBuffer_->GetInstancingData(); }
	std::vector<RayTracingInstance> CollectRTInstances(const RaytracingScene* scene) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;
	DxCommand* dxCommand_;

	std::unique_ptr<MeshRegistry> meshRegistry_;
	std::unique_ptr<InstancedMeshBuffer> instancedBuffer_;
};