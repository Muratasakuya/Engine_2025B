#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/DxSwapChain.h>
#include <Engine/Core/Graphics/PostProcess/RenderTexture.h>
#include <Engine/Core/Graphics/Descriptors/RTVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/DSVDescriptor.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>

// scene
#include <Engine/Core/Graphics/GPUObject/SceneConstBuffer.h>

// renderer
#include <Engine/Core/Graphics/Renderer/MeshRenderer.h>
#include <Engine/Core/Graphics/Renderer/SpriteRenderer.h>

// front
class SceneView;
class ECSManager;
class DxCommand;

//============================================================================
//	RenderEngine class
//============================================================================
class RenderEngine {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	//--------- enum class ---------------------------------------------------

	enum class ViewType {

		Main,  //メイン
		Debug, // デバッグ視点用
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	RenderEngine() = default;
	~RenderEngine() = default;

	void Init(ID3D12Device8* device, DxShaderCompiler* shaderCompiler, DxCommand* dxCommand);
	void CreateRenderTexture(class WinApp* winApp, ID3D12Device8* device, IDXGIFactory7* factory);

	// GPUの更新処理
	void UpdateGPUBuffer(SceneView* sceneView);

	// viewごとの描画
	void Rendering(ViewType type);

	// swapChain描画
	void BeginRenderFrameBuffer();
	void EndRenderFrameBuffer();

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ECSManager* ecsManager_;
	DxCommand* dxCommand_;

	std::unique_ptr<DxSwapChain> dxSwapChain_;

	// renderTexture
	std::unordered_map<ViewType, std::unique_ptr<RenderTexture>> renderTextures_;
	std::unique_ptr<GuiRenderTexture> guiRenderTexture_; // frameBufferからコピーする用

	// descriptor
	std::unique_ptr<RTVDescriptor> rtvDescriptor_;
	std::unique_ptr<DSVDescriptor> dsvDescriptor_;
	std::unique_ptr<SRVDescriptor> srvDescriptor_;

	// pipeline
	std::unique_ptr<PipelineState> skinningPipeline_;

	// scene
	std::unique_ptr<SceneConstBuffer> sceneBuffer_;

	// renderer
	std::unique_ptr<MeshRenderer> meshRenderer_;
	std::unique_ptr<SpriteRenderer> spriteRenderer_;

	//--------- functions ----------------------------------------------------

	// init
	void InitDescriptor(ID3D12Device8* device);
	void InitRenderTextrue(ID3D12Device8* device);
	void InitRenderer(ID3D12Device8* device, DxShaderCompiler* shaderCompiler);

	// render
	void Renderers(ViewType type);

	// command
	void BeginRenderTarget(RenderTexture* renderTexture);
	void EndRenderTarget(RenderTexture* renderTexture);
};