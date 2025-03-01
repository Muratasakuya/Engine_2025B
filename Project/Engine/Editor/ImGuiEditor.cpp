#include "ImGuiEditor.h"

//============================================================================
//	ImGuiEditor classMethods
//============================================================================

void ImGuiEditor::Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle) {

	renderTextureGPUHandle_ = renderTextureGPUHandle;

	// サイズの変更、移動不可
	windowFlag_ = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
}

void ImGuiEditor::Display() {

	// ドッキング設定
	ImGui::DockSpaceOverViewport
	(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

	// imguiの表示
	MainWindow();

	Hierarchy();

	Inspector();
}

void ImGuiEditor::MainWindow() {

	// 表示する画像サイズ
	const ImVec2 imageSize(864.0f, 486.0f);

	ImGui::Begin("Game");

	ImGui::Image(ImTextureID(renderTextureGPUHandle_.ptr), imageSize);
	ImGui::End();

	ImGui::Begin("Debug");

	ImGui::Image(ImTextureID(renderTextureGPUHandle_.ptr), imageSize);
	ImGui::End();
}

void ImGuiEditor::Hierarchy() {

	ImGui::Begin("Hierarchy");

	ImGui::End();
}

void ImGuiEditor::Inspector() {

	ImGui::Begin("Inspector");

	ImGui::End();
}