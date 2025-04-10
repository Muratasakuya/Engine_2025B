#pragma once

//============================================================================
//	include
//============================================================================

// directX
#include <d3d12.h>
// imgui
#include <imgui.h>
// front
class CameraManager;

//============================================================================
//	ImGuiEditor class
//============================================================================
class ImGuiEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ImGuiEditor() = default;
	~ImGuiEditor() = default;

	void Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle,
		const D3D12_GPU_DESCRIPTOR_HANDLE& debugSceneRenderTextureGPUHandle,
		const D3D12_GPU_DESCRIPTOR_HANDLE& shadowMapGPUHandle);

	void Display();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE debugSceneRenderTextureGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE shadowMapGPUHandle_;

	ImGuiWindowFlags windowFlag_;

	// imgui表示フラグ
	bool displayEnable_;

	//--------- functions ----------------------------------------------------

	// renderTextureの描画
	void MainWindow();

	// console
	void Console();

	// hierarchy
	void Hierarchy();

	// inspector
	void Inspector();
};