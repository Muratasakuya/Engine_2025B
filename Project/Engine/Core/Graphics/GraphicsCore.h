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
#include <Engine/Core/Graphics/PostProcess/ShadowMap.h>
#include <Engine/Core/Graphics/Descriptors/RTVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/DSVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/GPUObject/GPUObjectSystem.h>
#include <Engine/External/ImGuiManager.h>

// renderer
#include <Engine/Renderer/MeshRenderer.h>
#include <Engine/Renderer/EffectRenderer.h>
#include <Engine/Renderer/SpriteRenderer.h>

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

	void Init(uint32_t width, uint32_t height, class WinApp* winApp);

	void InitRenderer(class Asset* asset);

	void Finalize(HWND hwnd);

	//--------- rendering ----------------------------------------------------

	void BeginRenderFrame();

	void Render(class CameraManager* cameraManager,
		class LightManager* lightManager);

	//--------- accessor -----------------------------------------------------

	ID3D12Device8* GetDevice() const { return dxDevice_->Get(); }

	DxCommand* GetDxCommand() const { return dxCommand_.get(); }

	SRVDescriptor* GetSRVDescriptor() const { return srvDescriptor_.get(); }

	DxShaderCompiler* GetDxShaderCompiler() const { return dxShaderComplier_.get(); }

	GPUObjectSystem* GetGPUObjectSystem() const { return gpuObjectSystem_.get(); }

	PostProcessSystem* GetPostProcessSystem() const { return postProcessSystem_.get(); }

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

	//--------- directX ----------//

	std::unique_ptr<DxDevice> dxDevice_;

	std::unique_ptr<DxCommand> dxCommand_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;

	std::unique_ptr<DxShaderCompiler> dxShaderComplier_;

	std::unique_ptr<RenderTexture> renderTexture_;
	std::unique_ptr<GuiRenderTexture> guiRenderTexture_;

	std::unique_ptr<RenderTexture> debugSceneRenderTexture_;

	std::unique_ptr<ShadowMap> shadowMap_;

	std::unique_ptr<PostProcessSystem> postProcessSystem_;

	std::unique_ptr<MeshRenderer> meshRenderer_;
	std::unique_ptr<EffectRenderer> effectRenderer_;
	std::unique_ptr<SpriteRenderer> spriteRenderer_;

	std::unique_ptr<RTVDescriptor> rtvDescriptor_;
	std::unique_ptr<DSVDescriptor> dsvDescriptor_;
	std::unique_ptr<SRVDescriptor> srvDescriptor_;

	std::unique_ptr<ImGuiManager> imguiManager_;

	std::unique_ptr<GPUObjectSystem> gpuObjectSystem_;

	//--------- functions ----------------------------------------------------

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