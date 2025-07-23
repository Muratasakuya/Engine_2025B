#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Effect/Particle/Core/GPUParticleUpdater.h>
#include <Engine/Effect/Particle/Core/ParticleRenderer.h>
#include <Engine/Effect/Particle/ParticleSystem.h>

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
		SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler);

	void Update(DxCommand* dxCommand);

	void Rendering(bool debugEnable, SceneConstBuffer* sceneBuffer, DxCommand* dxCommand);

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

	// GPU
	std::unique_ptr<GPUParticleUpdater> gpuUpdater_;
	std::unique_ptr<ParticleRenderer> renderer_;

	std::vector<std::unique_ptr<ParticleSystem>> systems_;

	//--------- functions ----------------------------------------------------

	ParticleManager() : IGameEditor("ParticleManager") {}
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
};