#include "ParticleUpdateAlphaReferenceModule.h"

//============================================================================
//	ParticleUpdateAlphaReferenceModule classMethods
//============================================================================

void ParticleUpdateAlphaReferenceModule::Init() {

	// 初期値の設定
	useNoiseTexture_ = false;
	color_.start = 0.5f;
	color_.target = 0.5f;

	noise_.start = 0.0f;
	noise_.target = 0.5f;
}

void ParticleUpdateAlphaReferenceModule::Execute(
	CPUParticle::ParticleData& particle, [[maybe_unused]] float deltaTime) {

	// 色
	particle.material.alphaReference = std::lerp(
		color_.start, color_.target, EasedValue(easing_, particle.progress));

	// 使用しない場合は処理しない
	particle.textureInfo.useNoiseTexture = static_cast<int32_t>(useNoiseTexture_);
	if (!useNoiseTexture_) {
		return;
	}

	// ノイズ
	particle.material.noiseAlphaReference = std::lerp(
		noise_.start, noise_.target, EasedValue(easing_, particle.progress));
}

void ParticleUpdateAlphaReferenceModule::ImGui() {

	ImGui::Checkbox("useNoiseTexture", &useNoiseTexture_);

	ImGui::DragFloat("startColorReference", &color_.start, 0.01f);
	ImGui::DragFloat("targetColorReference", &color_.target, 0.01f);

	if (!useNoiseTexture_) {
		return;
	}

	ImGui::DragFloat("starttNoiseReference", &noise_.start, 0.01f);
	ImGui::DragFloat("targetNoiseReference", &noise_.target, 0.01f);

	Easing::SelectEasingType(easing_, GetName());
}