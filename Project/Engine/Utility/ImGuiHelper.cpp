#include "ImGuiHelper.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetEditor.h>

//============================================================================
//	ImGuiHelper classMethods
//============================================================================

void ImGuiHelper::ImageButtonWithLabel(const char* id,
	const std::string& label, ImTextureID textureId, const ImVec2& size) {

	ImGui::PushID(id);
	ImGui::BeginGroup();

	// テキストサイズ計算
	ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

	// 現在の位置取得
	ImVec2 pos = ImGui::GetCursorScreenPos();

	// 上にテキストの高さ分スペースを確保
	float labelSpacing = 2.0f;
	ImGui::Dummy(ImVec2(size.x, textSize.y + labelSpacing));

	// 画像ボタンの描画
	ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + textSize.y + labelSpacing));
	ImGui::ImageButton(label.c_str(), textureId, size);

	// テキストをボタンの上に中央揃えで描画
	ImVec2 textPos = ImVec2(
		pos.x + (size.x - textSize.x) * 0.5f,
		pos.y);
	ImGui::GetWindowDrawList()->AddText(
		textPos,
		ImGui::GetColorU32(ImGuiCol_Text),
		label.c_str());

	ImGui::EndGroup();
	ImGui::PopID();
}

const DragPayload* ImGuiHelper::DragDropPayload(PendingType expectedType) {

	if (!ImGui::BeginDragDropTarget()) {
		return nullptr;
	}

	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kDragPayloadId);
	if (payload) {
		auto* dragPayload = static_cast<const DragPayload*>(payload->Data);
		if (dragPayload && dragPayload->type == expectedType) {

			ImGui::EndDragDropTarget();
			return dragPayload;
		}
	}

	ImGui::EndDragDropTarget();
	return nullptr;
}