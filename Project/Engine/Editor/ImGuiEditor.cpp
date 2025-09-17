#include "ImGuiEditor.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Engine/Scene/SceneView.h>
#include <Lib/MathUtils/MathUtils.h>

// imgui表示
#include <Engine/Asset/AssetEditor.h>
#include <Engine/Editor/ImGuiObjectEditor.h>
#include <Engine/Editor/Manager/GameEditorManager.h>
#include <Engine/Utility/GameTimer.h>

// imgui
#include <ImGuizmo.h>

//============================================================================
//	ImGuiEditor classMethods
//============================================================================

void ImGuiEditor::Init(const D3D12_GPU_DESCRIPTOR_HANDLE& renderTextureGPUHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE& debugSceneRenderTextureGPUHandle) {

	renderTextureGPUHandle_ = renderTextureGPUHandle;
	debugSceneRenderTextureGPUHandle_ = debugSceneRenderTextureGPUHandle;

	// サイズの変更、移動不可
	windowFlag_ = ImGuiWindowFlags_None;

	// 初期状態は表示
	displayEnable_ = true;
	editMode_ = false;

	gameViewSize_ = ImVec2(832.0f, 486.0f);
	debugViewSize_ = ImVec2(832.0f, 486.0f);
}

void ImGuiEditor::Display(SceneView* sceneView) {

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
	// Guizmo
	ImGuizmo::BeginFrame();

	// layout操作
	EditLayout();

	// imguiの表示
	MainWindow(sceneView);

	Console();

	Hierarchy();

	Inspector();

	Asset();
}

void ImGuiEditor::EditLayout() {

	if (!editMode_) {
		return;
	}

	ImGui::Begin("EditLayout");

	ImGui::DragFloat2("gameViewSize", &gameViewSize_.x, 1.0f);
	ImGui::DragFloat2("debugViewSize", &debugViewSize_.x, 1.0f);

	ImGui::End();
}

void ImGuiEditor::MainWindow(SceneView* sceneView) {

	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove);

	ImGui::Image(ImTextureID(debugSceneRenderTextureGPUHandle_.ptr), debugViewSize_);

	SetInputArea(InputViewArea::Scene, ImGui::GetItemRectMin(), ImGui::GetItemRectSize());

	// ギズモ呼び出し
	GizmoContext gizmoContext{};
	gizmoContext.drawlist = ImGui::GetWindowDrawList();
	gizmoContext.rectMin = ImGui::GetItemRectMin();
	gizmoContext.rectSize = ImGui::GetItemRectSize();
	gizmoContext.orthographic = false;
	Math::ToColumnMajor(Matrix4x4::Transpose(sceneView->GetSceneCamera()->GetViewMatrix()), gizmoContext.view);
	Math::ToColumnMajor(Matrix4x4::Transpose(sceneView->GetSceneCamera()->GetProjectionMatrix()), gizmoContext.projection);
	ImGuiObjectEditor::GetInstance()->DrawManipulateGizmo(gizmoContext);

	ImGui::End();

	ImGui::Begin("Game", nullptr, windowFlag_);

	ImGui::Image(ImTextureID(renderTextureGPUHandle_.ptr), gameViewSize_);

	SetInputArea(InputViewArea::Game, ImGui::GetItemRectMin(), ImGui::GetItemRectSize());
	ImGui::End();
}

void ImGuiEditor::Console() {

	ImGui::Begin("Console");

	GameTimer::ImGui();

	ImGui::End();
}

void ImGuiEditor::Hierarchy() {

	ImGui::Begin("Hierarchy", nullptr, windowFlag_);

	if (ImGui::BeginTabBar("Hierarchy")) {

		if (ImGui::BeginTabItem("GameObject")) {

			// scene内のobject選択
			ImGuiObjectEditor::GetInstance()->SelectObject();
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
			ImGuiObjectEditor::GetInstance()->EditObject();
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

	//AssetEditor::GetInstance()->EditLayout();

	ImGui::Begin("Asset", nullptr, windowFlag_);

	AssetEditor::GetInstance()->ImGui();

	ImGui::End();
}

void ImGuiEditor::SetInputArea(InputViewArea viewArea, const ImVec2& imMin, const ImVec2& imSize) {

	Input* input = Input::GetInstance();

	Vector2 min = Vector2(imMin.x, imMin.y);
	Vector2 size = Vector2(imSize.x, imSize.y);
	input->SetViewRect(viewArea, min, size);
}