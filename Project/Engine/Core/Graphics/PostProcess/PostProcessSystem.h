#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessType.h>
#include <Engine/Core/Graphics/PostProcess/BloomProcessor.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBuffer.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessPipeline.h>
#include <Engine/Editor/Base/IGameEditor.h>
#include <Lib/MathUtils/Algorithm.h>

// c++
#include <vector>
#include <unordered_map>
// front
class SRVDescriptor;

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

	// postProcess作成
	void Create(const std::vector<PostProcess>& processes);

	// postProcess実行
	void Execute(class RenderTexture* inputTexture, class DxCommand* dxCommand);

	// 最終的なtextureをframeBufferに描画する
	void RenderFrameBuffer(class DxCommand* dxCommand);

	// imgui
	void ImGui() override;

	void ToWrite(class DxCommand* dxCommand);

	// processの追加、削除
	void AddProcess(PostProcess process);
	void RemoveProcess(PostProcess process);
	void ClearProcess();

	// postProcessに使うtextureの設定
	void InputProcessTexture(const std::string& textureName, PostProcess process, class Asset* asset);

	//--------- accessor -----------------------------------------------------

	template <typename T>
	void SetParameter(const T& parameter, PostProcess process);

	PostProcessPipeline* GetPipeline() const { return pipeline_.get(); }
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

	// pipeline
	std::unique_ptr<PostProcessPipeline> pipeline_;
	std::unique_ptr<PipelineState> offscreenPipeline_;

	std::vector<PostProcess> initProcesses_;   // 初期化済み
	std::vector<PostProcess> activeProcesses_; // 適用するプロセス
	bool bloomEnable_; // bloom処理を最後に行わせるためのフラグ

	// ブルーム処理、処理が長いのでブルーム用のクラスで処理を行う
	// ブルームは最後の実行する
	std::unique_ptr<BloomProcessor> bloom_;
	std::unordered_map<PostProcess, std::unique_ptr<ComputePostProcessor>> processors_;

	// buffers
	std::unordered_map<PostProcessType, std::unique_ptr<IPostProcessBuffer>> buffers_;

	// frameBufferに描画するGPUHandle、最終的な結果
	D3D12_GPU_DESCRIPTOR_HANDLE frameBufferGPUHandle_;

	//--------- functions ----------------------------------------------------

	void CreateCBuffer(PostProcessType type);
	void ExecuteCBuffer(ID3D12GraphicsCommandList* commandList, PostProcessType type);

	PostProcessType GetPostProcessType(PostProcess process) const;
};

//============================================================================
//	PostProcessSystem templateMethods
//============================================================================

template<typename T>
inline void PostProcessSystem::SetParameter(const T& parameter, PostProcess process) {

	PostProcessType type = GetPostProcessType(process);

	if (Algorithm::Find(buffers_, type, true)) {

		buffers_[type]->SetParameter((void*)&parameter, sizeof(T));
	}
}