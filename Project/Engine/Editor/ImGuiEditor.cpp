#include "ImGuiEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/EntityComponent.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	ImGuiEditor classMethods
//============================================================================

void ImGuiEditor::Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& debugSceneRenderTextureGPUHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& shadowMapGPUHandle) {

	renderTextureGPUHandle_ = renderTextureGPUHandle;
	debugSceneRenderTextureGPUHandle_ = debugSceneRenderTextureGPUHandle;
	shadowMapGPUHandle_ = shadowMapGPUHandle;

	// サイズの変更、移動不可
	windowFlag_ = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
}

void ImGuiEditor::Display() {

	// ドッキング設定
	ImGui::DockSpaceOverViewport
	(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

	// imguiの表示
	MainWindow();

	Engine();

	Hierarchy();

	Inspector();
}

void ImGuiEditor::MainWindow() {

	// 表示する画像サイズ
	const ImVec2 imageSize(864.0f, 486.0f);

	ImGui::Begin("Game", nullptr, windowFlag_);

	ImGui::Image(ImTextureID(renderTextureGPUHandle_.ptr), imageSize);
	ImGui::End();

	ImGui::Begin("Debug", nullptr, windowFlag_);

	ImGui::Image(ImTextureID(debugSceneRenderTextureGPUHandle_.ptr), imageSize);
	ImGui::End();

	ImGui::Begin("ShadowMap");

	ImGui::Image(ImTextureID(shadowMapGPUHandle_.ptr), ImVec2(486.0f, 486.0f));
	ImGui::End();
}

void ImGuiEditor::Engine() {

	ImGui::Begin("Engine");

	GameTimer::ImGui();

	ImGui::End();
}

void ImGuiEditor::Hierarchy() {

	ImGui::Begin("Hierarchy");

	EntityComponent::GetInstance()->ImGuiSelectObject();

	ImGui::End();
}

void ImGuiEditor::Inspector() {

	ImGui::Begin("Inspector");

	EntityComponent::GetInstance()->ImGuiEdit();

	ImGui::End();
}