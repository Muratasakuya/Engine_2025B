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
class SceneView;
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
		SceneView* sceneView);

	void Update();

	void Rendering(bool debugEnable, class SceneConstBuffer* sceneBuffer, ID3D12GraphicsCommandList6* commandList);

	void ImGui() override;

	void ResetParticleData();
	void ResetParticleData(const std::string& emitterName);

	// Game: user
	// emitter読み込み処理(初期化、作成)
	void LoadEmitter(const std::string& emitterName, const std::string& fileName);
	// 更新処理
	void UpdateEmitter(const std::string& emitterName);

	// 発生処理
	void Emit(const std::string& emitterName);
	// 一定間隔で発生
	void FrequencyEmit(const std::string& emitterName);

	// 各emitterへのsetter、発生させた後に設定する
	// 座標
	void SetTranslate(const std::string& emitterName, const Vector3& translate);
	// 回転
	void SetRotate(const std::string& emitterName, const Quaternion& rotate);

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
		DxStructuredBuffer<EffectMaterial> materialBuffer; // material
		DxStructuredBuffer<Matrix4x4> worldMatrixBuffer;   // matrix
		// instance数
		uint32_t numInstance;
	};

	//--------- variables ----------------------------------------------------

	static ParticleSystem* instance_;

	Asset* asset_;
	SRVDescriptor* srvDescriptor_;
	ID3D12Device8* device_;
	SceneView* sceneView_;

	// handler、editorによる追加、選択、削除
	std::unique_ptr<ParticleEmitterHandler> emitterHandler_;

	// renderer
	std::unique_ptr<PipelineState> pipeline_;

	// emitters
	// editor用
	std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> editorEmitters_;
	// game用
	std::unordered_map<std::string, std::unique_ptr<ParticleEmitter>> gameEmitters_;
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