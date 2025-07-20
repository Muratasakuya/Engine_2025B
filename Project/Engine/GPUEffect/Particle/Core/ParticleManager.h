#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/GPUObject/DxStructuredBuffer.h>
#include <Engine/Object/Data/Transform.h>
#include <Engine/GPUEffect/Particle/Structures/ParticlePrimitiveStructures.h>

// front
class Asset;
class SRVDescriptor;
class DxShaderCompiler;
class DxCommand;
class SceneView;

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

	void InitParticle(DxCommand* dxCommand);

	void Update(DxCommand* dxCommand);

	void Rendering(bool debugEnable, class SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	// singleton
	static ParticleManager* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structures ----------------------------------------------------

	// GPUParticleを理解するためにまずは実装する
	struct ParticleMaterial {

		Color color;
	};

	struct ParticleForGPU {

		Vector3 translation;
		Vector3 scale;

		float lifeTime;
		float currentTime;

		Vector3 velocity;
	};

	struct EmitterSphere {

		Vector3 translation;
		float radius;
		uint32_t count;
		float frequency;
		float frequencyTime;
		uint32_t emit;
	};

	struct PerFrame {

		float time;
		float deltaTime;
	};

	//--------- variables ----------------------------------------------------

	static ParticleManager* instance_;

	Asset* asset_;
	SceneView* sceneView_;

	const UINT kMaxInstanceCount_ = 1024;

	// pipeline
	// 計算
	std::unique_ptr<PipelineState> initParticlePipeline_;
	std::unique_ptr<PipelineState> emitParticlePipeline_;
	std::unique_ptr<PipelineState> updateParticlePipeline_;
	// 描画
	std::unique_ptr<PipelineState> renderPipeline_;

	std::vector<PlaneForGPU> planeInstances_;
	std::vector<ParticleMaterial> materialInstances_;

	// emitter
	EmitterSphere emitterSphere_;
	PerFrame perFrame_;

	// meshShader、pixelShaderで使用
	DxStructuredBuffer<PlaneForGPU> planeBuffer_;
	DxStructuredBuffer<ParticleMaterial> materialBuffer_;
	// computeShaderで使用
	DxStructuredBuffer<ParticleForGPU> particleBuffer_;
	DxStructuredBuffer<uint32_t> freeListIndexBuffer_;
	DxStructuredBuffer<uint32_t> freeListBuffer_;
	DxConstBuffer<EmitterSphere> emitterSphereBuffer_;
	DxConstBuffer<PerFrame> perFrameBuffer_;

	//--------- functions ----------------------------------------------------

	// update
	void UpdateEmitter();
	void EmitParticle(DxCommand* dxCommand);
	void UpdateParticle(DxCommand* dxCommand);

	ParticleManager() : IGameEditor("ParticleManager") {}
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
};