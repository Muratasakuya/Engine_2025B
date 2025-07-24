#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessType.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBuffer.h>
#include <Engine/Core/Graphics/PostProcess/ComputePostProcessor.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessPipeline.h>
#include <Engine/Editor/Base/IGameEditor.h>
#include <Lib/MathUtils/Algorithm.h>

// c++
#include <vector>
#include <unordered_map>
// front
class SRVDescriptor;
class SceneView;

//============================================================================
//	PostProcessSystem class
//============================================================================
class PostProcessSystem :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PostProcessSystem() : IGameEditor("postProcess") {};
	~PostProcessSystem() = default;

	void Init(ID3D12Device8* device, class DxShaderCompiler* shaderComplier,
		SRVDescriptor* srvDescriptor);

	void Update(SceneView* sceneView);

	// postProcess作成
	void Create(const std::vector<PostProcessType>& processes);

	// postProcess実行
	void Execute(const D3D12_GPU_DESCRIPTOR_HANDLE& inputSRVGPUHandle, class DxCommand* dxCommand);
	void ExecuteDebugScene(const D3D12_GPU_DESCRIPTOR_HANDLE& inputSRVGPUHandle, class DxCommand* dxCommand);

	// 最終的なtextureをframeBufferに描画する
	void RenderFrameBuffer(class DxCommand* dxCommand);

	// imgui
	void ImGui() override;

	void ToWrite(class DxCommand* dxCommand);

	// processの追加、削除
	void AddProcess(PostProcessType process);
	void RemoveProcess(PostProcessType process);
	void ClearProcess();

	// postProcessに使うtextureの設定
	void InputProcessTexture(const std::string& textureName, PostProcessType process, class Asset* asset);

	//--------- accessor -----------------------------------------------------

	template <typename T>
	void SetParameter(const T& parameter, PostProcessType process);
	void SetDepthFrameBufferGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { depthFrameBurferGPUHandle_ = handle; }

	PostProcessPipeline* GetPipeline() const { return pipeline_.get(); }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetDebugSceneGPUHandle() const { return debugSceneBloomProcessor_->GetSRVGPUHandle(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//- dependencies variables -----------------------------------------------

	uint32_t width_;
	uint32_t height_;
	ID3D12Device* device_;
	SRVDescriptor* srvDescriptor_;

	//--------- variables ----------------------------------------------------

	uint32_t bloomExecuteCount_;

	// pipeline
	std::unique_ptr<PostProcessPipeline> pipeline_;
	std::unique_ptr<PipelineState> offscreenPipeline_;

	std::vector<PostProcessType> initProcesses_;   // 初期化済み
	std::vector<PostProcessType> activeProcesses_; // 適用するプロセス

	// postProcess処理を行うmap
	std::unordered_map<PostProcessType, std::unique_ptr<ComputePostProcessor>> processors_;

	// debugScene用、bloomのみ
	std::unique_ptr<ComputePostProcessor> debugSceneBloomProcessor_;

	// buffers
	std::unordered_map<PostProcessType, std::unique_ptr<IPostProcessBuffer>> buffers_;

	// frameBufferに描画するGPUHandle、最終的な結果
	D3D12_GPU_DESCRIPTOR_HANDLE frameBufferGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE depthFrameBurferGPUHandle_;

	//--------- functions ----------------------------------------------------

	void CreateCBuffer(PostProcessType type);
	void ExecuteCBuffer(ID3D12GraphicsCommandList* commandList, PostProcessType type);
};

//============================================================================
//	PostProcessSystem templateMethods
//============================================================================

template<typename T>
inline void PostProcessSystem::SetParameter(const T& parameter, PostProcessType process) {

	if (Algorithm::Find(buffers_, process, true)) {

		buffers_[process]->SetParameter((void*)&parameter, sizeof(T));
	}
}