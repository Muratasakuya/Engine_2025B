#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Particle/ParticleEmitter.h>
#include <Engine/Particle/ParticleEmitterHandler.h>

// c++
#include <unordered_map>
// front
class CameraManager;
class SRVDescriptor;

//============================================================================
//	ParticleSystem class
//============================================================================
class ParticleSystem :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(Asset* asset, ID3D12Device8* device,
		SRVDescriptor* srvDescriptor, DxShaderCompiler* shaderCompiler,
		CameraManager* cameraManager);

	void Update();

	void Rendering(bool debugEnable, class SceneConstBuffer* sceneBuffer, ID3D12GraphicsCommandList6* commandList);

	void ImGui() override;
	//--------- accessor -----------------------------------------------------

	// singleton
	static ParticleSystem* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct RenderParticleData {

		// mesh情報
		EffectMesh* mesh;
		// structuredBuffer
		DxConstBuffer<EffectMaterial> materialBuffer; // material
		DxConstBuffer<Matrix4x4> worldMatrixBuffer;   // matrix
		// instance数
		uint32_t numInstance;
	};

	//--------- variables ----------------------------------------------------

	static ParticleSystem* instance_;

	Asset* asset_;
	SRVDescriptor* srvDescriptor_;
	ID3D12Device8* device_;
	CameraManager* cameraManager_;

	// handler、editorによる追加、選択、削除
	std::unique_ptr<ParticleEmitterHandler> emitterHandler_;

	// renderer
	std::unique_ptr<PipelineState> pipeline_;

	// emitters
	std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> emitters_;
	// blendMode別で分岐
	std::unordered_map<BlendMode, std::vector<RenderParticleData>> renderParticleData_;

	//--------- functions ----------------------------------------------------

	void CreateEmitter();

	void UpdateEmitter();

	ParticleSystem() : IGameEditor("ParticleSystem") {}
	~ParticleSystem() = default;
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem& operator=(const ParticleSystem&) = delete;
};