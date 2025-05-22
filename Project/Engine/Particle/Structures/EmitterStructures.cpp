#include "EmitterStructures.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	EmitterStructures classMethods
//============================================================================

void EmitterShape::SelectEmitterShape(EmitterShapeType& emitterShape, const std::string& label) {

	const char* shapeOptions[] = {
		"Sphere","Hemisphere","Box","Cone"
	};

	int shapeIndex = static_cast<int>(emitterShape);
	if (ImGui::BeginCombo(("EmitterShape##" + label).c_str(), shapeOptions[shapeIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(shapeOptions); i++) {

			const bool isSelected = (shapeIndex == i);
			if (ImGui::Selectable(shapeOptions[i], isSelected)) {

				shapeIndex = i;
				emitterShape = static_cast<EmitterShapeType>(i);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void EmitterShape::EditEmitterShape(const EmitterShapeType& emitterShape,
	EmitterSphere& sphere, EmitterHemisphere& hemisphere, EmitterBox& box, EmitterCone& cone) {

	// 分岐処理
	switch (emitterShape) {
	case EmitterShapeType::Sphere: {

		ImGui::DragFloat("radius", &sphere.radius, 0.01f);
		ImGui::DragFloat3("center", &sphere.center.x, 0.01f);
		break;
	}
	case EmitterShapeType::Hemisphere: {

		ImGui::DragFloat("radius", &hemisphere.radius, 0.01f);
		ImGui::DragFloat3("center", &hemisphere.center.x, 0.01f);
		ImGui::DragFloat3("eulerRotate", &hemisphere.eulerRotate.x, 0.001f);
		break;
	}
	case EmitterShapeType::Box: {

		ImGui::DragFloat3("center", &box.center.x, 0.01f);
		ImGui::DragFloat3("size", &box.size.x, 0.01f);
		ImGui::DragFloat3("eulerRotate", &box.eulerRotate.x, 0.001f);
		break;
	}
	case EmitterShapeType::Cone: {

		ImGui::DragFloat("baseRadius", &cone.baseRadius, 0.01f);
		ImGui::DragFloat("topRadius", &cone.topRadius, 0.01f);
		ImGui::DragFloat("height", &cone.height, 0.01f);
		ImGui::DragFloat3("center", &cone.center.x, 0.01f);
		ImGui::DragFloat3("eulerRotate", &cone.eulerRotate.x, 0.001f);
		break;
	}
	}
}