#include "ImGuiEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

// imgui表示
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Editor/ImGuiInspector.h>
#include <Engine/Editor/Manager/GameEditorManager.h>
#include <Engine/Scene/Camera/CameraManager.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	ImGuiEditor classMethods
//============================================================================

void ImGuiEditor::Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& debugSceneRenderTextureGPUHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& shadowMapGPUHandle) {

	renderTextureGPUHandle_ = renderTextureGPUHandle;
	debugSceneRenderTextureGPUHandle_ = debugSceneRenderTextureGPUHandle;

	// debug表示用、必要な時以外表示しない
	shadowMapGPUHandle_ = shadowMapGPUHandle;

	// サイズの変更、移動不可
	windowFlag_ =
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove;

	// 初期状態は表示
	displayEnable_ = true;
	editMode_ = false;

	gameViewSize_ = ImVec2(1088.0f, 612.0f);
	debugViewSize_ = ImVec2(1088.0f, 612.0f);
}

void ImGuiEditor::Display() {

	// imguiの表示切り替え
	// F11で行う
	if (displayEnable_) {
		if (Input::GetInstance()->TriggerKey(DIK_F10)) {

			displayEnable_ = false;
		}
	} else {

		if (Input::GetInstance()->TriggerKey(DIK_F10)) {

			displayEnable_ = true;
		}
	}

	if (!displayEnable_) {
		return;
	}

	// ドッキング設定
	ImGui::DockSpaceOverViewport
	(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

	// layout操作
	EditLayout();

	// imguiの表示
	MainWindow();

	Console();

	Hierarchy();

	Inspector();

	Asset();
}

void ImGuiEditor::EditLayout() {

	if (Input::GetInstance()->TriggerKey(DIK_1)) {

		editMode_ = !editMode_;
	}

	if (!editMode_) {
		return;
	}

	ImGui::Begin("EditLayout");

	ImGui::DragFloat2("gameViewSize", &gameViewSize_.x, 1.0f);
	ImGui::DragFloat2("debugViewSize", &debugViewSize_.x, 1.0f);

	ImGui::End();
}

void ImGuiEditor::MainWindow() {

	// 表示する画像サイズ
	const ImVec2 imageSize(1216.0f, 684.0f);

	ImGui::Begin("Game", nullptr, windowFlag_);

	ImGui::Image(ImTextureID(renderTextureGPUHandle_.ptr), gameViewSize_);
	ImGui::End();

	ImGui::Begin("Debug", nullptr, windowFlag_);

	ImGui::Image(ImTextureID(debugSceneRenderTextureGPUHandle_.ptr), debugViewSize_);
	ImGui::End();
}

void ImGuiEditor::Console() {

	ImGui::Begin("Console", nullptr, windowFlag_);

	GameTimer::ImGui();

	ImGui::End();
}

void ImGuiEditor::Hierarchy() {

	ImGui::Begin("Hierarchy", nullptr, windowFlag_);

	if (ImGui::BeginTabBar("Hierarchy")) {

		if (ImGui::BeginTabItem("GameObject")) {

			// scene内のobject選択
			ImGuiInspector::GetInstance()->SelectObject();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Editor")) {

			// editorの選択
			GameEditorManager::GetInstance()->SelectEditor();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ImGuiEditor::Inspector() {

	ImGui::Begin("Inspector", nullptr, windowFlag_);

	// 選択されたものの操作
	if (ImGui::BeginTabBar("Inspector")) {

		if (ImGui::BeginTabItem("GameObject")) {

			// scene内のobject選択
			ImGuiInspector::GetInstance()->EditObject();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Editor")) {

			// editorの選択
			GameEditorManager::GetInstance()->EditEditor();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void ImGuiEditor::Asset() {

	AssetEditor::GetInstance()->EditLayout();

	ImGui::Begin("Asset", nullptr, windowFlag_);

	AssetEditor::GetInstance()->ImGui();

	ImGui::End();
}