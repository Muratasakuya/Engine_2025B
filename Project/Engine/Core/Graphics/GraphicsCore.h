#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxDevice.h>
#include <Engine/Core/Graphics/DxCommand.h>
#include <Engine/Core/Graphics/DxSwapChain.h>
#include <Engine/Core/Graphics/Pipeline/DxShaderCompiler.h>
#include <Engine/Core/Graphics/Pipeline/PipelineState.h>
#include <Engine/Core/Graphics/Pipeline/MeshShaderPipelineState.h>
#include <Engine/Core/Graphics/PostProcess/Manager/PostProcessManager.h>
#include <Engine/Core/Graphics/PostProcess/RenderTexture.h>
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>

// test
#include <Engine/Test_MeshShader/MSInputAssembler.h>

// renderer
#include <Engine/Renderer/MeshRenderer.h>
#include <Engine/Renderer/SpriteRenderer.h>

// managers
#include <Engine/Renderer/Managers/RenderObjectManager.h>
#include <Engine/Core/Graphics/Managers/RTVManager.h>
#include <Engine/Core/Graphics/Managers/DSVManager.h>
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Engine/Core/Graphics/Managers/ImGuiManager.h>
// c++
#include <memory>
// front
class WinApp;
class CameraManager;

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

	void Init(uint32_t width, uint32_t height,
		WinApp* winApp, CameraManager* cameraManager);

	void Finalize(HWND hwnd);

	//--------- rendering ----------------------------------------------------

	void BeginRenderFrame();

	void Render();

	//--------- accessor -----------------------------------------------------

	ID3D12Device* GetDevice() const { return dxDevice_->Get(); }

	DxCommand* GetDxCommand() const { return dxCommand_.get(); }

	SRVManager* GetSRVManager() const { return srvManager_.get(); }

	DxShaderCompiler* GetDxShaderCompiler() const { return dxShaderComplier_.get(); }

	RenderObjectManager* GetRenderObjectManager() const { return renderObjectManager_.get(); }

	PostProcessManager* GetPostProcessManager() const { return postProcessManager_.get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetRenderTextureGPUHandle() const { return guiRenderTexture_->GetGPUHandle(); }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetDebugSceneRenderTextureGPUHandle() const { return debugSceneRenderTexture_->GetGPUHandle(); }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetShadowMapGPUHandle() const { return shadowMap_->GetGPUHandle(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	uint32_t windowWidth_;
	uint32_t windowHeight_;

	uint32_t shadowMapWidth_;
	uint32_t shadowMapHeight_;

	Color windowClearColor_;

	//--------- test -------------//

	CameraManager* cameraManager_;

	std::unique_ptr<MSInputAssembler> msTestInputAssembler_;
	std::unique_ptr<DxConstBuffer<MSTestTransformationMatrix>> msTestTransform_;

	//--------- directX ----------//

	std::unique_ptr<DxDevice> dxDevice_;

	std::unique_ptr<DxCommand> dxCommand_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;

	std::unique_ptr<DxShaderCompiler> dxShaderComplier_;

	std::unique_ptr<RenderTexture> renderTexture_;
	std::unique_ptr<GuiRenderTexture> guiRenderTexture_;

	std::unique_ptr<RenderTexture> debugSceneRenderTexture_;

	std::unique_ptr<ShadowMap> shadowMap_;

	std::unique_ptr<PostProcessManager> postProcessManager_;

	std::unique_ptr<MeshRenderer> meshRenderer_;
	std::unique_ptr<SpriteRenderer> spriteRenderer_;

	std::unique_ptr<RTVManager> rtvManager_;
	std::unique_ptr<DSVManager> dsvManager_;
	std::unique_ptr<SRVManager> srvManager_;

	std::unique_ptr<ImGuiManager> imguiManager_;

	std::unique_ptr<PipelineState> offscreenPipeline_;

	std::unique_ptr<MeshShaderPipelineState> meshShaderPipeline_;

	std::unique_ptr<RenderObjectManager> renderObjectManager_;

	//--------- functions ----------------------------------------------------

	// test //
	void RenderingTest();

	void InitDXDevice();

	// shadowMapへの描画処理
	void RenderZPass();

	// renderTextureへの描画処理
	void RenderOffscreenTexture();

	// debugSceneRenderTextureへの描画処理
	void RenderDebugSceneRenderTexture();

	// frameBufferへの描画処理
	void RenderFrameBuffer();

	void Renderers(bool debugEnable);

	void EndRenderFrame();
};