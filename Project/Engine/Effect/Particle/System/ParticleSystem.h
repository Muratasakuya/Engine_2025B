#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Effect/Particle/Data/GPUParticleGroup.h>
#include <Engine/Effect/Particle/Data/CPUParticleGroup.h>

// imgui
#include <imgui.h>
// front
class Asset;

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	template <typename T>
	struct NameGroup {

		std::string name;
		T group;
	};

	struct GroupHandle {

		ParticleType type;
		int index; 
	};

public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleSystem() = default;
	~ParticleSystem() = default;

	void Init(ID3D12Device* device, Asset* asset, const std::string& name);

	void Update();

	// editor
	void ImGuiSelectedGroupEditor();
	void ImGuiGroupAdd();
	void ImGuiGroupSelect();
	void ImGuiSystemParameter();

	//--------- accessor -----------------------------------------------------

	void SetName(const std::string& name) { name_ = name; }
	void SetGroupName(uint32_t i, const std::string& name) { gpuGroups_[i].name = name; }
	void SelectGroup(int index) { selected_.index = index; }

	const std::string& GetName() const { return name_; }
	const std::string& GetGroupName(uint32_t i) const { return gpuGroups_[i].name; }

	std::vector<NameGroup<GPUParticleGroup>>& GetGPUGroup() { return gpuGroups_; }
	std::vector<NameGroup<CPUParticleGroup>>& GetCPUGroup() { return cpuGroups_; }
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
	std::vector<NameGroup<GPUParticleGroup>> gpuGroups_;
	// CPU
	std::vector<NameGroup<CPUParticleGroup>> cpuGroups_;

	// editor
	ParticleType particleType_;
	ParticlePrimitiveType primitiveType_;
	GroupHandle selected_{ ParticleType::GPU, -1 };
	GroupHandle renaming_{ ParticleType::GPU, -1 };
	int nextGroupId_ = 0;         // グループ添え字インデックス
	char renameBuffer_[128] = {}; // 入力用バッファ
	// layout
	float comboWidth_ = 104.0f;
	float itemWidth_ = 160.0f;
	ImVec2 buttonSize_ = ImVec2(88.0f, 24.0f);

	//--------- functions ----------------------------------------------------

	// editor
	void AddGroup();
	void RemoveGroup();

	// helper
	void EditLayout();
};