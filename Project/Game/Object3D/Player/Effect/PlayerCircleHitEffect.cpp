#include "PlayerCircleHitEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerCircleHitEffect classMethods
//============================================================================

void PlayerCircleHitEffect::InitComponent() {

	uint32_t id = GameObjectHelper::CreateEffect(
		"effectDefaultPlane", "effectCircle", "PlayerCircleHitEffect", "Player");

	// 各component取得
	transform_ = Component::GetComponent<EffectTransformComponent>(id);
	material_ = Component::GetComponent<EffectMaterialComponent>(id);
	mesh_ = Component::GetComponent<PrimitiveMeshComponent>(id);

	// billboardを行う
	transform_->useBillboardMatrix_ = true;
	// 加算合成
	mesh_->SetBlendMode(BlendMode::kBlendModeAdd);
}

void PlayerCircleHitEffect::InitAnimation() {

	// animation初期化
	scaleAnimation_ = std::make_unique<SimpleAnimation<Vector3>>();
	alphaAnimation_ = std::make_unique<SimpleAnimation<float>>();
}

void PlayerCircleHitEffect::Init() {

	// 各componentの初期化
	InitComponent();

	// animationの初期化
	InitAnimation();

	// json適応
	ApplyJson();
}

void PlayerCircleHitEffect::UpdateAnimation() {

	// scale処理を行う
	scaleAnimation_->LerpValue(transform_->scale);
	// alpha処理を行う
	alphaAnimation_->LerpValue(material_->material.color.a);
}

void PlayerCircleHitEffect::ImGui() {

	if (ImGui::Button("Emit Effect", ImVec2(192.0f, 32.0f))) {

		emitEffect_ = !emitEffect_;
		scaleAnimation_->Start();
		alphaAnimation_->Start();
	}

	// 各parameterのimgui表示を行う
	transform_->ImGui(224.0f);
	material_->ImGui(224.0f);

	if (ImGui::CollapsingHeader("scaleAnimation")) {

		scaleAnimation_->ImGui();
	}

	if (ImGui::CollapsingHeader("alphaAnimation")) {

		alphaAnimation_->ImGui();
	}
}

void PlayerCircleHitEffect::Reset() {

	scaleAnimation_->Reset();
	alphaAnimation_->Reset();
	transform_->scale = Vector3::AnyInit(0.0f);
}

void PlayerCircleHitEffect::StartAnimation() {

	scaleAnimation_->Start();
	alphaAnimation_->Start();
}

void PlayerCircleHitEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/Effect/playerCircleHitEffect.json", data)) {
		return;
	}

	// transform設定
	transform_->FromJson(data["Transform"]);
	transform_->scale = Vector3::AnyInit(0.0f);
	// material設定
	material_->FromJson(data["Material"]);
	// animation設定
	scaleAnimation_->FromJson(data["ScaleAnimation"]);
	alphaAnimation_->FromJson(data["AlphaAnimation"]);
}

void PlayerCircleHitEffect::SaveJson() {

	Json data;

	// transform設定
	transform_->ToJson(data["Transform"]);
	// material保存
	material_->ToJson(data["Material"]);
	// animation保存
	scaleAnimation_->ToJson(data["ScaleAnimation"]);
	alphaAnimation_->ToJson(data["AlphaAnimation"]);

	JsonAdapter::Save("Player/Effect/playerCircleHitEffect.json", data);
}