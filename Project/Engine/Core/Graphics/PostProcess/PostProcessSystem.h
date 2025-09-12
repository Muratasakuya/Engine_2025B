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
class DxCommand;
class Asset;

//============================================================================
//	PostProcessSystem class
//============================================================================
class PostProcessSystem :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	void Init(ID3D12Device8* device, class DxShaderCompiler* shaderComplier,
		SRVDescriptor* srvDescriptor, Asset* asset);

	void Update(SceneView* sceneView);

	// postProcess作成
	void Create(const std::vector<PostProcessType>& processes);

	// postProcess実行
	void Execute(const D3D12_GPU_DESCRIPTOR_HANDLE& inputSRVGPUHandle, DxCommand* dxCommand);
	void ExecuteDebugScene(const D3D12_GPU_DESCRIPTOR_HANDLE& inputSRVGPUHandle,DxCommand* dxCommand);

	// 最終的なtextureをframeBufferに描画する
	void RenderFrameBuffer(DxCommand* dxCommand);

	// imgui
	void ImGui() override;

	void ToWrite(DxCommand* dxCommand);

	// processの追加、削除
	void AddProcess(PostProcessType process);
	void RemoveProcess(PostProcessType process);
	void ClearProcess();

	// postProcessに使うtextureの設定
	void InputProcessTexture(const std::string& textureName, PostProcessType process);

	//--------- accessor -----------------------------------------------------

	template <typename T>
	void SetParameter(const T& parameter, PostProcessType process);
	void SetDepthFrameBufferGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { depthFrameBurferGPUHandle_ = handle; }

	PostProcessPipeline* GetPipeline() const { return pipeline_.get(); }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetCopySRVGPUHandle() const { return copyTextureProcess_->GetSRVGPUHandle(); }

	// singleton
	static PostProcessSystem* GetInstance();
	static void Finalize();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//- dependencies variables -----------------------------------------------

	uint32_t width_;
	uint32_t height_;
	ID3D12Device* device_;
	SRVDescriptor* srvDescriptor_;
	Asset* asset_;

	//--------- variables ----------------------------------------------------

	static PostProcessSystem* instance_;

	// pipeline
	std::unique_ptr<PostProcessPipeline> pipeline_;
	std::unique_ptr<PipelineState> offscreenPipeline_;

	std::vector<PostProcessType> initProcesses_;   // 初期化済み
	std::vector<PostProcessType> activeProcesses_; // 適用するプロセス

	// postProcess処理を行うmap
	std::unordered_map<PostProcessType, std::unique_ptr<ComputePostProcessor>> processors_;

	// buffers
	std::unordered_map<PostProcessType, std::unique_ptr<IPostProcessBuffer>> buffers_;

	// debugSceneのα値を調整するためのプロセス
	std::unique_ptr<ComputePostProcessor> copyTextureProcess_;

	// frameBufferに描画するGPUHandle、最終的な結果
	D3D12_GPU_DESCRIPTOR_HANDLE frameBufferGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE depthFrameBurferGPUHandle_;

	// editor
	int selectedProcessIndex_ = -1; // 選択されているactiveProcessesのインデックス
	bool needSort_ = false;         // Drag&Drop完了フラグ

	//--------- functions ----------------------------------------------------

	void CreateCBuffer(PostProcessType type);
	void ExecuteCBuffer(ID3D12GraphicsCommandList* commandList, PostProcessType type);

	void BeginTransition(PostProcessType type, DxCommand* dxCommand);

	PostProcessSystem() :IGameEditor("PostProcessSystem") {}
	~PostProcessSystem() = default;
	PostProcessSystem(const PostProcessSystem&) = delete;
	PostProcessSystem& operator=(const PostProcessSystem&) = delete;
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