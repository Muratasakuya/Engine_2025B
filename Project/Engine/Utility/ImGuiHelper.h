#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetEditorPayloadData.h>

// c++
#include <string>
// imgui
#include <imgui.h>

//============================================================================
//	ImGuiHelper class
//============================================================================
class ImGuiHelper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ImGuiHelper() = default;
	~ImGuiHelper() = default;

	static void ImageButtonWithLabel(const char* id,
		const std::string& label, ImTextureID textureId, const ImVec2& size);

	static const DragPayload* DragDropPayload(PendingType expectedType);
};