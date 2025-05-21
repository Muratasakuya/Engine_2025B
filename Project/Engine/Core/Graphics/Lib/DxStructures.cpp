#include "DxStructures.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	DxStructures classMethods
//============================================================================

void Blend::SelectBlendMode(BlendMode& blendMode, const std::string& label) {

	const char* blendOptions[] = {
			"Normal","Add","Subtract","Multiply","Screen"
	};

	int blendIndex = static_cast<int>(blendMode);
	if (ImGui::BeginCombo(("BlendMode##" + label).c_str(), blendOptions[blendIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(blendOptions); i++) {

			const bool isSelected = (blendIndex == i);
			if (ImGui::Selectable(blendOptions[i], isSelected)) {

				blendIndex = i;
				blendMode = static_cast<BlendMode>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}