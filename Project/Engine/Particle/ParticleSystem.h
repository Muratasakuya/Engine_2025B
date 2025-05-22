#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Particle/ParticleEmitter.h>
#include <Engine/Particle/ParticleEmitterHandler.h>

// c++
#include <unordered_map>
// front
class CameraManager;

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(Asset* asset, ID3D12Device* device, CameraManager* cameraManager);

	void Update();

	void ImGui() override;
	//--------- accessor -----------------------------------------------------

	// singleton
	static ParticleSystem* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static ParticleSystem* instance_;

	Asset* asset_;
	ID3D12Device* device_;
	CameraManager* cameraManager_;

	// handler、editorによる追加、選択、削除
	std::unique_ptr<ParticleEmitterHandler> emitterHandler_;

	// emitters
	std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> emitters_;

	//--------- functions ----------------------------------------------------

	void CreateEmitter();

	void UpdateEmitter();

	ParticleSystem() : IGameEditor("ParticleSystem") {}
	~ParticleSystem() = default;
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;
};