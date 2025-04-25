#include "PlayerLightningHitEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerLightningHitEffect classMethods
//============================================================================

void PlayerLightningHitEffect::InitComponent() {

	uint32_t id = GameObjectHelper::CreateEffect(
		"sirialLightningEffectPlane", "sirialLightning_1",
		"PlayerLightningHitEffect", "Player");

	// 各component取得
	transform_ = Component::GetComponent<EffectTransformComponent>(id);
	material_ = Component::GetComponent<EffectMaterialComponent>(id);
	mesh_ = Component::GetComponent<PrimitiveMeshComponent>(id);

	// billboardを行う
	transform_->useBillboardMatrix_ = true;
	// 加算合成
	mesh_->SetBlendMode(BlendMode::kBlendModeAdd);
}

void PlayerLightningHitEffect::InitAnimation() {

	// animation初期化
	scaleAnimation_ = std::make_unique<SimpleAnimation<Vector3>>();
	uvScrollAnimation_ = std::make_unique<SimpleAnimation<float>>();
	alphaAnimation_ = std::make_unique<SimpleAnimation<float>>();

	uvScrollAnimation_->move_.moveValue = 1.0f / 16.0f;
}

void PlayerLightningHitEffect::Init() {

	// 各componentの初期化
	InitComponent();

	// animationの初期化
	InitAnimation();

	// json適応
	ApplyJson();
}

void PlayerLightningHitEffect::UpdateAnimation() {

	if (!uvScrollAnimation_->loop_.isStart) {
		Reset();
	}

	// scale処理を行う
	scaleAnimation_->LerpValue(transform_->scale);
	// uvScroll処理を行う
	uvScrollAnimation_->MoveValue(material_->uvTransform.translate.x);
	// alpha処理を行う
	alphaAnimation_->LerpValue(material_->material.color.a);

	if (material_->uvTransform.translate.x >= 1.0f) {

		Reset();
	}
}

void PlayerLightningHitEffect::ImGui() {

	if (ImGui::Button("Emit Effect", ImVec2(192.0f, 32.0f))) {

		emitEffect_ = !emitEffect_;
		scaleAnimation_->Start();
		uvScrollAnimation_->Start();
	}

	// 各parameterのimgui表示を行う
	transform_->ImGui(224.0f);
	material_->ImGui(224.0f);

	if (ImGui::CollapsingHeader("scaleAnimation")) {

		scaleAnimation_->ImGui();
	}

	if (ImGui::CollapsingHeader("uvScrollAnimation")) {

		uvScrollAnimation_->ImGui();
	}
}

void PlayerLightningHitEffect::Reset() {

	scaleAnimation_->Reset();
	alphaAnimation_->Reset();
	uvScrollAnimation_->Reset();

	material_->material.color.a = 0.0f;
	material_->uvTransform.translate.x = 0.0f;
}

void PlayerLightningHitEffect::SaveJson() {

	Json data;

	// transform設定
	transform_->ToJson(data["Transform"]);
	// material保存
	material_->ToJson(data["Material"]);
	// animation保存
	scaleAnimation_->ToJson(data["ScaleAnimation"]);
	uvScrollAnimation_->ToJson(data["UvScrollAnimation"]);

	JsonAdapter::Save("Player/Effect/playerLightningHitEffect.json", data);
}

void PlayerLightningHitEffect::StartAnimation() {

	scaleAnimation_->Start();
	uvScrollAnimation_->Start();
	alphaAnimation_->Start();

	material_->material.color.a = 1.0f;
}

void PlayerLightningHitEffect::SetParent(const BaseTransform& parent) {

	transform_->parent = &parent;
}

void PlayerLightningHitEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/Effect/playerLightningHitEffect.json", data)) {
		return;
	}

	// transform設定
	transform_->FromJson(data["Transform"]);
	// material設定
	material_->FromJson(data["Material"]);
	material_->material.color.a = 0.0f;
	// animation設定
	scaleAnimation_->FromJson(data["ScaleAnimation"]);
	uvScrollAnimation_->FromJson(data["UvScrollAnimation"]);

	if (!JsonAdapter::LoadCheck("Player/Effect/playerCircleHitEffect.json", data)) {
		return;
	}

	alphaAnimation_->FromJson(data["AlphaAnimation"]);
}