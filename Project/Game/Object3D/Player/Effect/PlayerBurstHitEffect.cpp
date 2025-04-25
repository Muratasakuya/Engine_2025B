#include "PlayerBurstHitEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerHitEffect classMethods
//============================================================================

void PlayerBurstHitEffect::InitComponent() {

	uint32_t id = GameObjectHelper::CreateEffect(
		"sirialHitEffectPlane", "sirialHitEffect", "PlayerBurstHitEffect", "Player");

	// 各component取得
	transform_ = Component::GetComponent<EffectTransformComponent>(id);
	material_ = Component::GetComponent<EffectMaterialComponent>(id);
	mesh_ = Component::GetComponent<PrimitiveMeshComponent>(id);

	// billboardを行う
	transform_->useBillboardMatrix_ = true;
	// 加算合成
	mesh_->SetBlendMode(BlendMode::kBlendModeAdd);
}

void PlayerBurstHitEffect::InitAnimation() {

	// animation初期化
	uvScrollAnimation_ = std::make_unique<SimpleAnimation<float>>();
}

void PlayerBurstHitEffect::Init() {

	// 各componentの初期化
	InitComponent();

	// animationの初期化
	InitAnimation();

	// json適応
	ApplyJson();
}

void PlayerBurstHitEffect::UpdateAnimation() {

	if (!uvScrollAnimation_->loop_.isStart) {
		Reset();
	}

	// uvScroll処理を行う
	uvScrollAnimation_->MoveValue(material_->uvTransform.translate.x);

	if (material_->uvTransform.translate.x >= 1.0f) {

		Reset();
	}
}

void PlayerBurstHitEffect::ImGui() {

	if (ImGui::Button("Emit Effect", ImVec2(192.0f, 32.0f))) {

		emitEffect_ = !emitEffect_;
		uvScrollAnimation_->Start();
	}

	// 各parameterのimgui表示を行う
	transform_->ImGui(224.0f);
	material_->ImGui(224.0f);
	uvScrollAnimation_->ImGui();
}

void PlayerBurstHitEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/Effect/playerBurstHitEffect.json", data)) {
		return;
	}

	// transform設定
	transform_->FromJson(data);
	// material設定
	material_->FromJson(data);
	material_->material.color.a = 0.0f;
	// animation設定
	uvScrollAnimation_->FromJson(data);
}

void PlayerBurstHitEffect::SaveJson() {

	Json data;

	// transform設定
	transform_->ToJson(data);
	// material保存
	material_->ToJson(data);
	// animation保存
	uvScrollAnimation_->ToJson(data);

	JsonAdapter::Save("Player/Effect/playerBurstHitEffect.json", data);
}

void PlayerBurstHitEffect::Reset() {

	uvScrollAnimation_->Reset();
	material_->material.color.a = 0.0f;
	material_->uvTransform.translate.x = 0.0f;
}

void PlayerBurstHitEffect::StartAnimation() {

	uvScrollAnimation_->Start();
	material_->material.color.a = 1.0f;
}