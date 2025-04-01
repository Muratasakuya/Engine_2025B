#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/PostProcess/PostProcessType.h>
#include <Engine/Core/Graphics/PostProcess/BloomProcessor.h>
#include <Engine/Core/Graphics/PostProcess/Buffer/PostProcessBuffer.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessPipelineManager.h>
#include <Lib/MathUtils/Algorithm.h>

// c++
#include <vector>
#include <unordered_map>
// front
class SRVManager;

//============================================================================
//	PostProcessManager class
//============================================================================
class PostProcessManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PostProcessManager() = default;
	~PostProcessManager() = default;

	void Init(ID3D12Device* device, class DxShaderCompiler* shaderComplier,
		SRVManager* srvManager, uint32_t width, uint32_t height);

	// postProcess作成
	void Create(const std::vector<PostProcess>& processes);

	// postProcess実行
	void Execute(class RenderTexture* inputTexture, class DxCommand* dxCommand);

	// 最終的なtextureをframeBufferに描画する
	void RenderFrameBuffer(class PipelineState* pipeline,
		class DxCommand* dxCommand);

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
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//- dependencies variables -----------------------------------------------

	uint32_t width_;
	uint32_t height_;
	ID3D12Device* device_;
	SRVManager* srvManager_;

	//--------- variables ----------------------------------------------------

	// pipeline
	std::unique_ptr<PostProcessPipelineManager> pipelineManager_;

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
//	PostProcessManager templateMethods
//============================================================================

template<typename T>
inline void PostProcessManager::SetParameter(const T& parameter, PostProcess process) {

	PostProcessType type = GetPostProcessType(process);

	if (Algorithm::Find(buffers_, type, true)) {

		buffers_[type]->SetParameter((void*)&parameter, sizeof(T));
	}
}