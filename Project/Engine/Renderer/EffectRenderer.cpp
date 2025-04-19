#include "EffectRenderer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/Context/MeshCommandContext.h>

//============================================================================
//	EffectRenderer classMethods
//============================================================================

void EffectRenderer::Init(ID3D12Device8* device,
	DxShaderCompiler* shaderCompiler, SRVDescriptor* srvDescriptor) {

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	// pipeline作成
	meshShaderPipeline_ = std::make_unique<PipelineState>();
	meshShaderPipeline_->Create("EffectMesh.json", device, srvDescriptor, shaderCompiler);
}

void EffectRenderer::UpdateBuffers(const std::vector<EffectForGPU>& effectBuffers) {

	// blendModeの変更確認
	const bool blendModeChanged = UpdateBlendMode(effectBuffers);

	// blendMode、bufferの数に変更がない場合は更新しない
	if (!blendModeChanged && bufferCount_ == effectBuffers.size()) {
		return;
	}

	// クリア
	buffers_.clear();

	// blendModeごとに分ける
	for (const auto& buffer : effectBuffers) {

		const BlendMode blendMode = buffer.primitiveMesh->GetBlendMode();
		buffers_[blendMode].push_back(buffer);
	}

	bufferCount_ = effectBuffers.size();
}

bool EffectRenderer::UpdateBlendMode(const std::vector<EffectForGPU>& effectBuffers) {

	// サイズが変わったら変更
	if (prevBlendModes_.size() != effectBuffers.size())
	{
		// prevBlendModesに現在のblendModeを入れる
		prevBlendModes_.clear();
		for (const auto& buffer : effectBuffers) {

			prevBlendModes_.push_back(buffer.primitiveMesh->GetBlendMode());
		}
		return true;
	}

	// blendModeの比較
	bool isChanged = false;
	for (size_t i = 0; i < effectBuffers.size(); ++i) {

		BlendMode currentBlendMode = effectBuffers[i].primitiveMesh->GetBlendMode();

		// 変更が合ったら変更する
		if (prevBlendModes_[i] != currentBlendMode) {

			prevBlendModes_[i] = currentBlendMode;
			isChanged = true;
		}
	}

	return isChanged;
}

void EffectRenderer::Rendering(bool debugEnable, GPUObjectSystem* gpuObjectSystem,
	ID3D12GraphicsCommandList6* commandList) {

	// 描画情報取得
	const auto& effectBuffers = gpuObjectSystem->GetEffectBuffers();
	MeshCommandContext commandContext{};

	if (effectBuffers.empty()) {
		return;
	}

	// bufferをblendModeごとに分ける処理
	UpdateBuffers(effectBuffers);

	// blendModeごとに描画
	for (const auto& [blendMode, buffers] : buffers_) {

		// renderTextureへの描画処理
		// pipeline設定
		commandList->SetGraphicsRootSignature(meshShaderPipeline_->GetRootSignature());
		commandList->SetPipelineState(meshShaderPipeline_->GetGraphicsPipeline(blendMode));

		// 共通のbuffer設定
		gpuObjectSystem->GetSceneBuffer()->SetViewProCommand(debugEnable, commandList);
		// allTexture
		commandList->SetGraphicsRootDescriptorTable(6,
			srvDescriptor_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

		for (const auto& buffer : buffers) {

			// matrix設定
			commandList->SetGraphicsRootConstantBufferView(4,
				buffer.matrix.GetResource()->GetGPUVirtualAddress());

			// material設定
			commandList->SetGraphicsRootConstantBufferView(7,
				buffer.material.GetResource()->GetGPUVirtualAddress());

			commandContext.DispatchMesh(commandList, buffer.primitiveMesh);
		}
	}
}