#include "ParticleValue.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	ParticleValue classMethods
//============================================================================

void ParticleBillboard::SelectBillboardType(ParticleBillboardType& billboardType, const std::string& label) {

	const char* billboardOptions[] = {
			"None","All","YAxis"
	};

	int billboardIndex = static_cast<int>(billboardType);
	if (ImGui::BeginCombo(("BillboardType##" + label).c_str(), billboardOptions[billboardIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(billboardOptions); i++) {

			const bool isSelected = (billboardIndex == i);
			if (ImGui::Selectable(billboardOptions[i], isSelected)) {

				billboardIndex = i;
				billboardType = static_cast<ParticleBillboardType>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}