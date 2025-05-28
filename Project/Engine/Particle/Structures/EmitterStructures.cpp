#include "EmitterStructures.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

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

void EmitterShape::Aplly(const Json& data, EmitterSphere& sphere,
	EmitterHemisphere& hemisphere, EmitterBox& box, EmitterCone& cone) {

	sphere.radius = data["Sphere"]["radius"];
	sphere.center = JsonAdapter::ToObject<Vector3>(data["Sphere"]["center"]);

	hemisphere.radius = data["Hemisphere"]["radius"];
	hemisphere.center = JsonAdapter::ToObject<Vector3>(data["Hemisphere"]["center"]);
	hemisphere.eulerRotate = JsonAdapter::ToObject<Vector3>(data["Hemisphere"]["eulerRotate"]);
	// 回転をquaternionに直す
	hemisphere.rotation = Quaternion::EulerToQuaternion(hemisphere.eulerRotate);

	box.size = JsonAdapter::ToObject<Vector3>(data["Box"]["size"]);
	box.center = JsonAdapter::ToObject<Vector3>(data["Box"]["center"]);
	box.eulerRotate = JsonAdapter::ToObject<Vector3>(data["Box"]["eulerRotate"]);
	// 回転をquaternionに直す
	box.rotation = Quaternion::EulerToQuaternion(hemisphere.eulerRotate);

	cone.baseRadius = data["Cone"]["baseRadius"];
	cone.topRadius = data["Cone"]["topRadius"];
	cone.height = data["Cone"]["height"];
	cone.center = JsonAdapter::ToObject<Vector3>(data["Cone"]["center"]);
	cone.eulerRotate = JsonAdapter::ToObject<Vector3>(data["Cone"]["eulerRotate"]);
	// 回転をquaternionに直す
	cone.rotation = Quaternion::EulerToQuaternion(hemisphere.eulerRotate);
}

void EmitterShape::Save(Json& data, const  EmitterSphere& sphere,
	const EmitterHemisphere& hemisphere, const  EmitterBox& box, const EmitterCone& cone) {

	data["Sphere"]["radius"] = sphere.radius;
	data["Sphere"]["center"] = JsonAdapter::FromObject<Vector3>(sphere.center);

	data["Hemisphere"]["radius"] = hemisphere.radius;
	data["Hemisphere"]["center"] = JsonAdapter::FromObject<Vector3>(hemisphere.center);
	data["Hemisphere"]["eulerRotate"] = JsonAdapter::FromObject<Vector3>(hemisphere.eulerRotate);

	data["Box"]["size"] = JsonAdapter::FromObject<Vector3>(box.size);
	data["Box"]["center"] = JsonAdapter::FromObject<Vector3>(box.center);
	data["Box"]["eulerRotate"] = JsonAdapter::FromObject<Vector3>(box.eulerRotate);

	data["Cone"]["baseRadius"] = cone.baseRadius;
	data["Cone"]["topRadius"] = cone.topRadius;
	data["Cone"]["height"] = cone.height;
	data["Cone"]["center"] = JsonAdapter::FromObject<Vector3>(cone.center);
	data["Cone"]["eulerRotate"] = JsonAdapter::FromObject<Vector3>(cone.eulerRotate);
}