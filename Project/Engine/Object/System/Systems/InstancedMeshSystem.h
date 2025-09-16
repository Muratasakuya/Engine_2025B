#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Async/AssetLoadWorker.h>
#include <Engine/Object/System/Base/ISystem.h>
#include <Engine/Core/Graphics/Mesh/MeshRegistry.h>
#include <Engine/Core/Graphics/GPUObject/InstancedMeshBuffer.h>
#include <Engine/Core/Graphics/Raytracing/RaytracingStructures.h>
#include <Engine/Scene/Methods/IScene.h>

// directX
#include <d3d12.h>
// c++
#include <unordered_set>
#include <xatomic.h>
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
	~InstancedMeshSystem();

	// 非同期処理
	void StartBuildWorker();
	void StopBuildWorker();

	void RequestBuild(const std::string& modelName,
		uint32_t maxInstStatic = Config::kMaxInstanceNum,
		uint32_t maxInstSkinned = 16);
	void RequestBuildForScene(Scene scene);

	// createBuffer
	void CreateStaticMesh(const std::string& modelName);
	void CreateSkinnedMesh(const std::string& modelName);

	Archetype Signature() const override;
	void Update(ObjectPoolManager& ObjectPoolManager) override;

	//--------- accessor -----------------------------------------------------

	const std::unordered_map<std::string, std::unique_ptr<IMesh>>& GetMeshes() const { return meshRegistry_->GetMeshes(); }
	const std::unordered_map<std::string, MeshInstancingData>& GetInstancingData() const { return instancedBuffer_->GetInstancingData(); }
	std::vector<RayTracingInstance> CollectRTInstances(const RaytracingScene* scene) const;

	bool IsReady(const std::string& name) const;
	bool IsBuilding() const;
	float GetBuildProgressForScene(Scene scene) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct MeshBuildJob {

		std::string name;     // 名前
		bool skinned;         // 骨の有無
		uint32_t maxInstance; // 最大数
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;
	DxCommand* dxCommand_;

	std::unique_ptr<MeshRegistry> meshRegistry_;
	std::unique_ptr<InstancedMeshBuffer> instancedBuffer_;
	std::unordered_map<std::string, std::vector<uint32_t>> objectIDsPerModel_;

	AssetLoadWorker<MeshBuildJob> buildWorker_;
	// 重複処理回避用
	std::mutex instancedMutex_;
	std::unordered_set<std::string> requested_;

	// 進捗カウンタ
	std::atomic<uint32_t> pendingJobs_{};
	std::atomic<uint32_t> runningJobs_{};
};