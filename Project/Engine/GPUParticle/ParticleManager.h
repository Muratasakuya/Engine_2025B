#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxConstBuffer.h>
#include <Engine/Object/Data/Transform.h>
#include <Lib/MathUtils/MathUtils.h>

// front
class Asset;
class SRVDescriptor;
class DxShaderCompiler;
class SceneView;

//============================================================================
//	structures
//============================================================================

struct PlaneForGPU {

	Vector2 size;
	Matrix4x4 world;
};
struct RingForGPU {

	float outerRadius;
	float innerRadius;
	int divide;
	Matrix4x4 world;
};
struct ParticleMaterial {

	Color color;
	Matrix4x4 uvTransform;
};

// 形状
enum class ParticleShape {

	Plane,
	Ring,
	Count
};

//============================================================================
//	ParticleManager class
//============================================================================
class ParticleManager :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(Asset* asset, ID3D12Device8* device,
		SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler,
		SceneView* sceneView);

	void Update();

	void Rendering(bool debugEnable, class SceneConstBuffer* sceneBuffer, ID3D12GraphicsCommandList6* commandList);

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	// singleton
	static ParticleManager* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ParticleManager* instance_;

	Asset* asset_;
	SceneView* sceneView_;

	std::unordered_map<ParticleShape, std::unique_ptr<PipelineState>> pipelines_;

	std::vector<PlaneForGPU> planeInstances_;
	Transform3D planeTransform_;
	std::vector<RingForGPU> ringInstances_;
	Transform3D ringTransform_;
	std::vector<ParticleMaterial> materialInstances_;

	// debug
	DxConstBuffer<PlaneForGPU> planeBuffer_;
	DxConstBuffer<RingForGPU> ringBuffer_;
	DxConstBuffer<ParticleMaterial> materialBuffer_;

	//--------- functions ----------------------------------------------------

	ParticleManager() : IGameEditor("ParticleManager") {}
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
};