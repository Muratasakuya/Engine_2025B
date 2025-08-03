#pragma once

//============================================================================
//	AssetEditorPayloadData
//============================================================================

static constexpr const char* kDragPayloadId = "ASSET_PATH";

// ファイルの種類
enum class PendingType {

	None,
	Texture,
	Model
};
struct DragPayload {

	PendingType type; // タイプ
	char name[260];   // 名前
};