#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Input/InputStructures.h>

// directX
#include <d3d12.h>
// imgui
#include <imgui.h>
// front
class SceneView;

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
		const D3D12_GPU_DESCRIPTOR_HANDLE& debugSceneRenderTextureGPUHandle);

	void Display(SceneView* sceneView);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE debugSceneRenderTextureGPUHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE shadowTextureGPUHandle_;

	ImGuiWindowFlags windowFlag_;

	// imgui表示フラグ
	bool displayEnable_;

	// parameter
	bool editMode_;
	ImVec2 gameViewSize_;
	ImVec2 debugViewSize_;

	//--------- functions ----------------------------------------------------

	// layoutEditor
	void EditLayout();

	// renderTextureの描画
	void MainWindow(SceneView* sceneView);

	// console
	void Console();

	// hierarchy
	void Hierarchy();

	// inspector
	void Inspector();

	// asset
	void Asset();

	// helper
	void SetInputArea(InputViewArea viewArea, const ImVec2& imMin, const ImVec2& imSize);
};