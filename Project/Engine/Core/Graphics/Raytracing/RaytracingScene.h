#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Raytracing/BottomLevelAS.h>
#include <Engine/Core/Graphics/Raytracing/TopLevelAS.h>

// c++
#include <unordered_map>
#include <memory>

//============================================================================
//	RaytracingScene class
//============================================================================
class RaytracingScene {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RaytracingScene() = default;
	~RaytracingScene() = default;

	void Init(ID3D12Device8* device);

	void BuildBLASes(ID3D12GraphicsCommandList6* commandList, const std::vector<IMesh*>& meshes);
	void BuildTLAS(ID3D12GraphicsCommandList6* commandList,
		const std::vector<RayTracingInstance>& instances);

	//--------- accessor -----------------------------------------------------

	ID3D12Resource* GetBLASResource(IMesh* mesh, uint32_t meshCount) const;
	ID3D12Resource* GetTLASResource() const { return tlas_.GetResource(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct MeshKey {
		IMesh* ptr;
		uint32_t subMesh;
		bool operator==(const MeshKey&) const = default;
	};
	struct MeshKeyHash {
		size_t operator()(const MeshKey& k) const noexcept {
			return std::hash<IMesh*>{}(k.ptr) ^ (k.subMesh * 0x9e3779b97f4a7c15ULL);
		}
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device8* device_;

	std::unordered_map<MeshKey, BottomLevelAS, MeshKeyHash> blases_;
	TopLevelAS tlas_;

	bool first_;
};