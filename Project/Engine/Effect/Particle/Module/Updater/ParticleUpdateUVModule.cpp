#include "ParticleUpdateUVModule.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/EnumAdapter.h>

//============================================================================
//	ParticleUpdateUVModule classMethods
//============================================================================

void ParticleUpdateUVModule::Init() {

	// 初期化値
	updateType_ = UpdateType::Lerp;
}

void ParticleUpdateUVModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	Vector3 translation{};
	switch (updateType_) {
	case ParticleUpdateUVModule::UpdateType::Lerp:

		// UV座標補間
		translation = Vector3::Lerp(translation_.start,
			translation_.target, EasedValue(easing_, particle.progress));
		break;
	case ParticleUpdateUVModule::UpdateType::Scroll:

		// UVスクロール
		translation = particle.material.uvTransform.GetTranslationValue();
		translation.x += scrollValue_.x;
		translation.y += scrollValue_.y;
		break;
	}

	// 回転
	float rotationZ = std::lerp(rotation_.start,
		rotation_.target, EasedValue(easing_, particle.progress));

	// uvMatrixの更新
	particle.material.uvTransform = Matrix4x4::MakeAffineMatrix(
		Vector3::AnyInit(1.0f), Vector3(0.0f, 0.0f, rotationZ), translation);
}

void ParticleUpdateUVModule::ImGui() {

	EnumAdapter<UpdateType>::Combo("updateType", &updateType_);

	ImGui::SeparatorText("Translation");

	switch (updateType_) {
	case ParticleUpdateUVModule::UpdateType::Lerp:

		ImGui::DragFloat2("startTranslation", &translation_.start.x, 0.01f);
		ImGui::DragFloat2("targetTranslation", &translation_.target.x, 0.01f);

		Easing::SelectEasingType(easing_, GetName());
		break;
	case ParticleUpdateUVModule::UpdateType::Scroll:

		ImGui::DragFloat2("scrollValue", &scrollValue_.x, 0.01f);
		break;
	}

	ImGui::SeparatorText("Rotation");

	ImGui::DragFloat("startRotation", &rotation_.start, 0.01f);
	ImGui::DragFloat("targetRotation", &rotation_.target, 0.01f);
}

Json ParticleUpdateUVModule::ToJson() {

	Json data;

	data["updateType"] = EnumAdapter<UpdateType>::ToString(updateType_);
	data["easingType"] = EnumAdapter<EasingType>::ToString(easing_);

	data["translation"]["start"] = translation_.start.ToJson();
	data["translation"]["target"] = translation_.target.ToJson();

	data["rotation"]["start"] = rotation_.start;
	data["rotation"]["target"] = rotation_.target;

	data["scrollValue"] = scrollValue_.ToJson();

	return data;
}

void ParticleUpdateUVModule::FromJson(const Json& data) {

	const auto& updateType = EnumAdapter<UpdateType>::FromString(data.value("updateType", ""));
	updateType_ = updateType.value();

	const auto& easingType = EnumAdapter<EasingType>::FromString(data.value("easingType", ""));
	easing_ = easingType.value();

	const auto& translationData = data["translation"];
	translation_.start = translation_.start.FromJson(translationData["start"]);
	translation_.target = translation_.target.FromJson(translationData["target"]);

	if (data.contains("rotation")) {

		rotation_.start = data["rotation"]["start"];
		rotation_.target = data["rotation"]["target"];
	}

	scrollValue_ = scrollValue_.FromJson(data["scrollValue"]);
}