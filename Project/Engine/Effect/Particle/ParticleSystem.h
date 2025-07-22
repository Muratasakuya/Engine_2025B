#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Data/GPUParticleGroup.h>
#include <Engine/Effect/Particle/Data/CPUParticleGroup.h>

// front
class Asset;

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleSystem() = default;
	~ParticleSystem() = default;

	void Init(ID3D12Device* device, Asset* asset, const std::string& name);

	void Update();

	void ImGui();

	//--------- accessor -----------------------------------------------------

	const std::vector<GPUParticleGroup>& GetGPUGroup() const { return gpuParticleGroups_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	Asset* asset_;

	// system名
	std::string name_;

	// GPU
	std::vector<GPUParticleGroup> gpuParticleGroups_;
	// CPU
	std::vector<CPUParticleGroup> cpuParticleGroups_;

	// editor
	ParticleType particleType_;
	ParticlePrimitiveType primitiveType_;

	//--------- functions ----------------------------------------------------

	// GPU: create
	void CreateGPUParticle();
	void CreateCPUParticle();

	// editor
	void AddParticle();
	void SelectParticle();
	void EditParticle();
};