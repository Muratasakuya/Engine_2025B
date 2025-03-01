#pragma once

//============================================================================
//	include
//============================================================================

// directX
#include <d3d12.h>
// imgui
#include <imgui.h>

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

	void Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle);

	void Display();

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUHandle_;

	ImGuiWindowFlags windowFlag_;

	//--------- functions ----------------------------------------------------

	// renderTextureの描画
	void MainWindow();

	// hierarchy
	void Hierarchy();

	// inspector
	void Inspector();

};