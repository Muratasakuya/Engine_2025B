#include "PostProcessManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Context/PostProcessCommandContext.h>
#include <Engine/Core/Graphics/PostProcess/RenderTexture.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Asset/Asset.h>

//============================================================================
//	PostProcessManager classMethods
//============================================================================

void PostProcessManager::AddProcess(PostProcess process) {

	// 追加
	if (Algorithm::Find(initProcesses_, process, true)) {

		activeProcesses_.push_back(process);
	}
}

void PostProcessManager::RemoveProcess(PostProcess process) {

	// 削除
	if (Algorithm::Find(activeProcesses_, process, true)) {

		activeProcesses_.erase(std::remove(activeProcesses_.begin(),
			activeProcesses_.end(), process), activeProcesses_.end());
	}
}

void PostProcessManager::ClearProcess() {

	// 全て削除
	activeProcesses_.clear();
}

void PostProcessManager::InputProcessTexture(
	const std::string& textureName, PostProcess process, Asset* asset) {

	// texture設定
	if (Algorithm::Find(initProcesses_, process, true)) {

		processors_[process]->SetProcessTexureGPUHandle(asset->GetGPUHandle(textureName));
	}
}

void PostProcessManager::Init(ID3D12Device8* device, DxShaderCompiler* shaderComplier,
	SRVManager* srvManager, uint32_t width, uint32_t height) {

	width_ = width;
	height_ = height;

	device_ = nullptr;
	device_ = device;

	srvManager_ = nullptr;
	srvManager_ = srvManager;

	bloomEnable_ = false;

	// pipeline初期化
	pipelineManager_ = std::make_unique<PostProcessPipelineManager>();
	pipelineManager_->Init(device, srvManager, shaderComplier);

	// offscreenPipeline初期化
	offscreenPipeline_ = std::make_unique<PipelineState>();
	offscreenPipeline_->Create("CopyTexture.json", device, srvManager, shaderComplier);
}

void PostProcessManager::Create(const std::vector<PostProcess>& processes) {

	if (!initProcesses_.empty() || processes.empty()) {
		return;
	}

	// 使用可能なprocessを入れる
	initProcesses_.assign(processes.begin(), processes.end());

	// 使用できるPostProcessを初期化する
	for (const auto& process : initProcesses_) {
		if (process == PostProcess::Bloom) {

			bloom_ = std::make_unique<BloomProcessor>();
			bloom_->Init(device_, srvManager_, width_, height_);
		} else {

			processors_[process] = std::make_unique<ComputePostProcessor>();
			processors_[process]->Init(device_, srvManager_, width_, height_);

			PostProcessType type = GetPostProcessType(process);
			CreateCBuffer(type);
		}
	}
}

void PostProcessManager::Execute(RenderTexture* inputTexture, DxCommand* dxCommand) {

	if (activeProcesses_.empty()) {

		// 使用するpostProcessがない場合はそのままinputTextureを返す
		frameBufferGPUHandle_ = inputTexture->GetGPUHandle();
		return;
	}

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);
	PostProcessCommandContext commandContext{};
	// 入力画像のGPUHandle取得
	D3D12_GPU_DESCRIPTOR_HANDLE inputGPUHandle = inputTexture->GetGPUHandle();

	for (const auto& process : activeProcesses_) {

		// ブルームは最後に行う
		if (process == PostProcess::Bloom) {

			bloomEnable_ = true;
			continue;
		} else {

			bloomEnable_ = false;
		}

		PostProcessType type = GetPostProcessType(process);

		if (processors_.find(process) != processors_.end()) {

			// pipeline設定
			pipelineManager_->SetPipeline(commandList, type);
			// buffer設定
			ExecuteCBuffer(commandList, type);
			// 実行
			commandContext.Execute(type, commandList,
				processors_[process].get(), inputGPUHandle);

			if (bloomEnable_) {

				// UnorderedAccess -> ComputeShader
				dxCommand->TransitionBarriers({ processors_[process]->GetOutputTextureResource() },
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
					D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			} else {
				if (process == activeProcesses_.back()) {

					// 最後の処理はframeBufferに描画するためにPixelShaderに遷移させる
					// UnorderedAccess -> PixelShader
					dxCommand->TransitionBarriers({ processors_[process]->GetOutputTextureResource() },
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				} else {

					// UnorderedAccess -> ComputeShader
					dxCommand->TransitionBarriers({ processors_[process]->GetOutputTextureResource() },
						D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
						D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
				}
			}

			// 出力を次のpostProcessの入力にする
			inputGPUHandle.ptr = NULL;
			inputGPUHandle = processors_[process]->GetSRVGPUHandle();
		}
	}

	frameBufferGPUHandle_.ptr = NULL;
	// 最終的なframeBufferに設定するGPUHandleの設定
	if (bloomEnable_) {

		bloom_->Execute(dxCommand, pipelineManager_.get(), inputGPUHandle);
		frameBufferGPUHandle_ = bloom_->GetGPUHandle();
	} else {

		frameBufferGPUHandle_ = processors_[activeProcesses_.back()]->GetSRVGPUHandle();
	}
}

void PostProcessManager::RenderFrameBuffer(DxCommand* dxCommand) {

	auto commandList = dxCommand->GetCommandList(CommandListType::Graphics);

	// frameBufferへの描画
	commandList->SetGraphicsRootSignature(offscreenPipeline_->GetRootSignature());
	commandList->SetPipelineState(offscreenPipeline_->GetGraphicsPipeline());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->SetGraphicsRootDescriptorTable(0, frameBufferGPUHandle_);

	const UINT vertexCount = 3;
	commandList->DrawInstanced(vertexCount, 1, 0, 0);
}

void PostProcessManager::ImGui() {

	// 各bufferのimgui表示
	for (const auto& buffer : std::views::values(buffers_)) {

		buffer->ImGui();
	}

	if (Algorithm::Find(activeProcesses_, PostProcess::Bloom)) {

		bloom_->ImGui();
	}
}

void PostProcessManager::ToWrite(DxCommand* dxCommand) {

	if (activeProcesses_.empty()) {
		return;
	}

	for (const auto& process : activeProcesses_) {

		if (process == PostProcess::Bloom) {
			continue;
		}
		if (bloomEnable_) {

			// ComputeShader -> UnorderedAccess
			dxCommand->TransitionBarriers(
				{ processors_[process]->GetOutputTextureResource() },
				D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		} else {
			if (process == activeProcesses_.back()) {

				// PixelShader -> UnorderedAccess
				dxCommand->TransitionBarriers(
					{ processors_[process]->GetOutputTextureResource() },
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			} else {

				// ComputeShader -> UnorderedAccess
				dxCommand->TransitionBarriers(
					{ processors_[process]->GetOutputTextureResource() },
					D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			}
		}
	}
	if (bloomEnable_) {

		bloom_->ToWrite(dxCommand);
	}
}

void PostProcessManager::CreateCBuffer(PostProcessType type) {

	switch (type) {
	case PostProcessType::HorizontalBlur: {

		auto buffer = std::make_unique<PostProcessBuffer<HorizonBlurForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::VerticalBlur: {

		auto buffer = std::make_unique<PostProcessBuffer<VerticalBlurForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::RadialBlur: {

		auto buffer = std::make_unique<PostProcessBuffer<RadialBlurForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::GaussianFilter: {

		auto buffer = std::make_unique<PostProcessBuffer<GaussianFilterForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::BoxFilter: {
		// bufferなし
		break;
	}
	case PostProcessType::Dissolve: {

		auto buffer = std::make_unique<PostProcessBuffer<DissolveForGPU>>();
		buffer->Init(device_, 3);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::Random: {
		// bufferなし、未実装
		break;
	}
	case PostProcessType::Vignette: {

		auto buffer = std::make_unique<PostProcessBuffer<VignetteForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::Grayscale: {
		// bufferなし
		break;
	}
	case PostProcessType::SepiaTone: {
		// bufferなし
		break;
	}
	case PostProcessType::LuminanceBasedOutline: {

		auto buffer = std::make_unique<PostProcessBuffer<LuminanceBasedOutlineForGPU>>();
		buffer->Init(device_, 2);

		buffers_[type] = std::move(buffer);
		break;
	}
	case PostProcessType::DepthBasedOutline: {

		auto buffer = std::make_unique<PostProcessBuffer<DepthBasedOutlineForGPU>>();
		buffer->Init(device_, 3);

		buffers_[type] = std::move(buffer);
		break;
	}
	}
}

void PostProcessManager::ExecuteCBuffer(
	ID3D12GraphicsCommandList* commandList, PostProcessType type) {

	if (Algorithm::Find(buffers_, type)) {

		// buffer更新
		buffers_[type]->Update();

		UINT rootIndex;
		D3D12_GPU_VIRTUAL_ADDRESS adress{};

		rootIndex = buffers_[type]->GetRootIndex();
		adress = buffers_[type]->GetResource()->GetGPUVirtualAddress();

		commandList->SetComputeRootConstantBufferView(rootIndex, adress);
	}
}

PostProcessType PostProcessManager::GetPostProcessType(PostProcess process) const {

	// userが指定したprocessのenumはpipelineのenumとは
	// 異なるのでpipeline用に変更する
	switch (process) {
	case PostProcess::HorizontalBlur: return PostProcessType::HorizontalBlur;
	case PostProcess::VerticalBlur: return PostProcessType::VerticalBlur;
	case PostProcess::Vignette: return PostProcessType::Vignette;
	case PostProcess::BoxFilter: return PostProcessType::BoxFilter;
	case PostProcess::Grayscale: return PostProcessType::Grayscale;
	case PostProcess::SepiaTone: return PostProcessType::SepiaTone;
	case PostProcess::GaussianFilter: return PostProcessType::GaussianFilter;
	case PostProcess::LuminanceBasedOutline: return PostProcessType::LuminanceBasedOutline;
	case PostProcess::DepthBasedOutline: return PostProcessType::DepthBasedOutline;
	case PostProcess::RadialBlur: return PostProcessType::RadialBlur;
	case PostProcess::Dissolve: return PostProcessType::Dissolve;
	case PostProcess::Random: return PostProcessType::Random;
	default:

		assert(false);
		return PostProcessType::BloomCombine;
	}
}