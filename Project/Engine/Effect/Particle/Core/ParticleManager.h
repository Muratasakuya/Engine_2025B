#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Effect/Particle/Core/GPUParticleUpdater.h>
#include <Engine/Effect/Particle/Core/ParticleRenderer.h>
#include <Engine/Effect/Particle/System/ParticleSystem.h>

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

	Asset* asset_;
	ID3D12Device8* device_;

	// GPU
	std::unique_ptr<GPUParticleUpdater> gpuUpdater_;
	std::unique_ptr<ParticleRenderer> renderer_;

	std::vector<std::unique_ptr<ParticleSystem>> systems_;

	// editor
	int nextSystemId_ = 0;        // システム添え字インデックス
	int selectedSystem_ = -1;     // 現在選択しているシステム
	int renamingSystem_ = -1;     // 改名中のシステムインデックス
	char renameBuffer_[128] = {}; // 入力用バッファ
	// layout
	float leftColumnWidth_ = 220.0f;
	ImVec2 leftUpChildSize_ = ImVec2(leftColumnWidth_, 120.0f);
	ImVec2 leftCenterChildSize_ = ImVec2(leftColumnWidth_, 120.0f);
	float rightUpChildSizeY_ = leftUpChildSize_.y;
	float rightCenterChildSizeY_ = leftCenterChildSize_.y;

	//--------- functions ----------------------------------------------------

	// init
	void RegisterModules();

	// editor
	void AddSystem();
	void RemoveSystem();

	void DrawLeftChild();
	void DrawRightChild();

	void DrawSystemAdd();
	void DrawSystemSelect();

	// helper
	void EditLayout();
	bool IsSystemSelected() const;

	ParticleManager() : IGameEditor("ParticleManager") {}
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
};