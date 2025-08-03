#include "ParticleUpdateScaleModule.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/EnumAdapter.h>

//============================================================================
//	ParticleUpdateScaleModule classMethods
//============================================================================

void ParticleUpdateScaleModule::Init() {

	// 初期化値
	scale_.start = Vector3::AnyInit(1.0f);
	scale_.target = Vector3::AnyInit(0.0f);
}

void ParticleUpdateScaleModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// t値取得
	const float lerpT = LoopedT(particle.progress);

	// 色を補間
	particle.transform.scale = Vector3::Lerp(scale_.start, scale_.target,
		EasedValue(easing_, lerpT));
}

void ParticleUpdateScaleModule::ImGui() {

	ImGui::DragFloat3("startScale", &scale_.start.x, 0.01f);
	ImGui::DragFloat3("targetScale", &scale_.target.x, 0.01f);

	Easing::SelectEasingType(easing_, GetName());

	ImGuiLoopParam();
}

Json ParticleUpdateScaleModule::ToJson() {

	Json data;

	// ループ
	ParticleLoopableModule::ToLoopJson(data);

	data["scale"]["start"] = scale_.start.ToJson();
	data["scale"]["target"] = scale_.target.ToJson();
	data["easingType"] = EnumAdapter<EasingType>::ToString(easing_);

	return data;
}

void ParticleUpdateScaleModule::FromJson(const Json& data) {

	// ループ
	ParticleLoopableModule::FromLoopJson(data);

	const auto& easingType = EnumAdapter<EasingType>::FromString(data.value("easingType", ""));
	easing_ = easingType.value();

	const auto& scaleData = data["scale"];
	scale_.start = scale_.start.FromJson(scaleData["start"]);
	scale_.target = scale_.target.FromJson(scaleData["target"]);
}