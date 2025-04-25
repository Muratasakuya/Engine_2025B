#include "PlayerLightningHitEffect.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerLightningHitEffect classMethods
//============================================================================

void PlayerLightningHitEffect::InitAnimation() {

	// animation初期化
	scaleAnimation_ = std::make_unique<SimpleAnimation<Vector3>>();
	uvScrollAnimation_ = std::make_unique<SimpleAnimation<float>>();
}

void PlayerLightningHitEffect::Init() {

	// 作成
	BasePrimitiveEffect::Init("sirialLightningEffectPlane", "sirialLightning_1",
		"PlayerLightningHitEffect", "Player");

	// 初期化後初期状態にしておく
	Reset();
}

void PlayerLightningHitEffect::StartAnimation(const Vector3& translate) {

	scaleAnimation_->Start();
	uvScrollAnimation_->Start();

	// 発生座標を指定
	transform_->translation = translate;
}

void PlayerLightningHitEffect::Reset() {

	scaleAnimation_->Reset();
	uvScrollAnimation_->Reset();

	// animationをしていない間は見えないようにする
	BasePrimitiveEffect::OutsideEffect();
	// uvScroll位置を元に戻す
	material_->uvTransform.translate.Init();
}

void PlayerLightningHitEffect::UpdateAnimation() {

	// scale処理を行う
	scaleAnimation_->LerpValue(transform_->scale);
	// uvScroll処理を行う
	uvScrollAnimation_->MoveValue(material_->uvTransform.translate.x);
}

void PlayerLightningHitEffect::ImGui() {

	BasePrimitiveEffect::EditComponent();

	if (ImGui::CollapsingHeader("scaleAnimation")) {

		scaleAnimation_->ImGui("scaleAnimation");
	}

	if (ImGui::CollapsingHeader("uvScrollAnimation")) {

		uvScrollAnimation_->ImGui("uvScrollAnimation");
	}
}

void PlayerLightningHitEffect::SaveJson() {

	Json data;

	// animation保存
	scaleAnimation_->ToJson(data["ScaleAnimation"]);
	uvScrollAnimation_->ToJson(data["UvScrollAnimation"]);

	// component保存
	BasePrimitiveEffect::SaveComponent(data);

	JsonAdapter::Save("Player/Effect/playerLightningHitEffect.json", data);
}

void PlayerLightningHitEffect::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/Effect/playerLightningHitEffect.json", data)) {
		return;
	}

	// animation設定
	scaleAnimation_->FromJson(data["ScaleAnimation"]);
	uvScrollAnimation_->FromJson(data["UvScrollAnimation"]);

	// component設定
	BasePrimitiveEffect::ApplyComponent(data);
}