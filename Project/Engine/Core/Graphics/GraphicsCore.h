#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxDevice.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/DxSwapChain.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/PostProcess/PostProcessSystem.h>
#include <Engine/Core/Graphics/PostProcess/RenderTexture.h>
#include <Engine/Core/Graphics/PostProcess/DepthTexture.h>
#include <Engine/Core/Graphics/Descriptors/RTVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/DSVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>
#include <Engine/External/ImGuiManager.h>

// renderer
#include <Engine/Core/Graphics/Renderer/MeshRenderer.h>
#include <Engine/Core/Graphics/Renderer/SpriteRenderer.h>

// c++
#include <memory>

//============================================================================
//	GraphicsCore class
//============================================================================
class GraphicsCore {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GraphicsCore() = default;
	~GraphicsCore() = default;

	void Init(class WinApp* winApp);

	void Finalize(HWND hwnd);

	//--------- rendering ----------------------------------------------------

	void BeginFrame();

	void Render(class CameraManager* cameraManager,
		class LightManager* lightManager);

	void DebugUpdate();

	//--------- accessor -----------------------------------------------------

	ID3D12Device8* GetDevice() const { return dxDevice_->Get(); }

	DxCommand* GetDxCommand() const { return dxCommand_.get(); }

	SRVDescriptor* GetSRVDescriptor() const { return srvDescriptor_.get(); }

	DxShaderCompiler* GetDxShaderCompiler() const { return dxShaderComplier_.get(); }

	PostProcessSystem* GetPostProcessSystem() const { return postProcessSystem_.get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRenderTextureGPUHandle() const { return guiRenderTexture_->GetGPUHandle(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetDebugSceneRenderTextureGPUHandle() const { return postProcessSystem_->GetDebugSceneGPUHandle(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- directX ----------//

	// dxObject
	std::unique_ptr<DxDevice> dxDevice_;
	std::unique_ptr<DxCommand> dxCommand_;
	std::unique_ptr<DxSwapChain> dxSwapChain_;
	std::unique_ptr<DxShaderCompiler> dxShaderComplier_;

	// renderTexture
	std::unique_ptr<RenderTexture> renderTexture_;
	std::unique_ptr<GuiRenderTexture> guiRenderTexture_;
	std::unique_ptr<RenderTexture> debugSceneRenderTexture_;

	// postProcess
	std::unique_ptr<PostProcessSystem> postProcessSystem_;

	std::unique_ptr<PipelineState> skinningPipeline_;
	std::unique_ptr<MeshRenderer> meshRenderer_;
	std::unique_ptr<SpriteRenderer> spriteRenderer_;

	// descriptor
	std::unique_ptr<RTVDescriptor> rtvDescriptor_;
	std::unique_ptr<DSVDescriptor> dsvDescriptor_;
	std::unique_ptr<SRVDescriptor> srvDescriptor_;

	std::unique_ptr<ImGuiManager> imguiManager_;

	std::unique_ptr<SceneConstBuffer> sceneBuffer_;

	//--------- functions ----------------------------------------------------

	void InitDXDevice();
	void InitRenderTexture();

	// renderTextureへの描画処理
	void RenderOffscreenTexture();

	// debugSceneRenderTextureへの描画処理
	void RenderDebugSceneRenderTexture();

	// frameBufferへの描画処理
	void RenderFrameBuffer();

	void Renderers(bool debugEnable);

	void EndRenderFrame();
};